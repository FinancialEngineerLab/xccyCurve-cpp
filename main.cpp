#include <iostream>
#include <iomanip>
#include <cmath>
#include <map>
#include <numeric>

#include <ql/qldefines.hpp>
#if !defined(BOOST_ALL_NO_LIB) && defined(BOOST_MSVC)
#  include <ql/auto_link.hpp>
#endif

#include <ql/quantlib.hpp>

#include "xccycurve.hpp"

using namespace QuantLib;

Size numRows = 5;
Size numCols = 5;

Integer swapLengths[] = {
      1,     2,     3,     4,     5};
Volatility swaptionVols[] = {
    0.3556,    0.3742,    0.3734,    0.3664,    0.3561,
    0.3936,    0.3901,    0.3802,    0.3682,    0.3557,
    0.3834,    0.3728,    0.3643,    0.3560,    0.3471,
    0.3643,    0.3502,    0.3407,    0.3306,   0.3202,
    0.3378,    0.3261,    0.3174,    0.3082,    0.2994};

void calibrateModel(
          const ext::shared_ptr<ShortRateModel>& model,
          const std::vector<ext::shared_ptr<BlackCalibrationHelper>>& swaptions) {

    std::vector<ext::shared_ptr<CalibrationHelper>> helpers(swaptions.begin(), swaptions.end());
    LevenbergMarquardt om;
    model->calibrate(helpers, om,
                     EndCriteria(400, 100, 1.0e-8, 1.0e-8, 1.0e-8));

    // Output the implied Black volatilities
    for (Size i=0; i<numRows; i++) {
        Size j = numCols - i -1; // 1x5, 2x4, 3x3, 4x2, 5x1
        Size k = i*numCols + j;
        Real npv = swaptions[i]->modelValue();
        Volatility implied = swaptions[i]->impliedVolatility(npv, 1e-4,
                                                             1000, 0.05, 0.50);
        Volatility diff = implied - swaptionVols[k];

        std::cout << i+1 << "x" << swapLengths[j]
                  << std::setprecision(5) << std::noshowpos
                  << ": model " << std::setw(7) << io::volatility(implied)
                  << ", market " << std::setw(7)
                  << io::volatility(swaptionVols[k])
                  << " (" << std::setw(7) << std::showpos
                  << io::volatility(diff) << std::noshowpos << ")\n";
    }
}


int main(int argc, const char * argv[])
{
    try
    {
        
        std::cout << "Switchabla Note ! " << std::endl;
        
        Calendar calendar = TARGET();
        Date settlementDate(30, Aug, 2023);
        Settings::instance().evaluationDate() = settlementDate;
        DayCounter dayCounter = Actual365Fixed();

        
        //1. Market Data: available bumping //
        
        // SOFR swap curve //
        
        DayCounter termStructureDayCounter = Actual365Fixed();
        std::vector<ext::shared_ptr<RateHelper>> sofrInstruments;

        // Date: ?? //
        std::map<Period, ext::shared_ptr<Quote>> longOisQuotes =
        {
            {1 * Months, ext::make_shared<SimpleQuote>(0.05331)},
            {3 * Months, ext::make_shared<SimpleQuote>(0.05417)},
            {6 * Months, ext::make_shared<SimpleQuote>(0.05494)},
            {12 * Months, ext::make_shared<SimpleQuote>(0.05480)},
            {24 * Months, ext::make_shared<SimpleQuote>(0.04949)},
            {36 * Months, ext::make_shared<SimpleQuote>(0.04958)},
            {48 * Months, ext::make_shared<SimpleQuote>(0.04371)},
            {60 * Months, ext::make_shared<SimpleQuote>(0.04231)},
            {84 * Months, ext::make_shared<SimpleQuote>(0.04068)}
        };
        
        auto sofr = ext::make_shared<Sofr>();
        
        for (const auto& q : longOisQuotes) {
            auto tenor = q.first;
            auto quote = q.second;
            auto helper = ext::make_shared<OISRateHelper>(
                2, tenor, Handle<Quote>(quote), sofr);
            sofrInstruments.push_back(helper);
        }

        auto oisSofrTermStructure = ext::make_shared<PiecewiseYieldCurve<Discount, LogLinear>>(settlementDate, sofrInstruments, termStructureDayCounter);

        oisSofrTermStructure->enableExtrapolation();

        // This curve will be used for discounting cash flows
        RelinkableHandle<YieldTermStructure> discountingTermStructure;
        discountingTermStructure.linkTo(oisSofrTermStructure);
        
        std::cout << "forward rate e.g. = " << discountingTermStructure->forwardRate(0.5,1,Continuous) << std::endl;
        
        
        // 2. Bermudan Swaption (and also Vanilla Swap) Setting //
        Date maturity = calendar.advance( settlementDate, 3*Years);
        Schedule contSchedule(settlementDate, maturity,
                                      Period(Quarterly),
                                      calendar, ModifiedFollowing,
                              ModifiedFollowing,
                                      DateGeneration::Forward,false);
        
        std::cout << "coupon schedule e.g" << std::endl;
        std::cout << "# of schedules! " << contSchedule.size() << std::endl;
        for(int i=0; i< contSchedule.size();i++)
        {
            std::cout << contSchedule.at(i).serialNumber() << ", " << contSchedule.dates().at(i) << std::endl;
        }
        
        // 3. Zero Coupon Bond Portfolio //
        
        
        
        // 4. Replicate Switchable Note //
        
        // 5. Issuer Greeks //
        
        // 6. Model Change ! //
             
    
        /**********************/
        std::map<Period, ext::shared_ptr<Quote>> longtwdQuotes =
            {
                {12 * Months, ext::make_shared<SimpleQuote>(0.0150)},
                {24 * Months, ext::make_shared<SimpleQuote>(0.0150)},
                {36 * Months, ext::make_shared<SimpleQuote>(0.0150)},
                {48 * Months, ext::make_shared<SimpleQuote>(0.0150)},
                {60 * Months, ext::make_shared<SimpleQuote>(0.0150)},
                {84 * Months, ext::make_shared<SimpleQuote>(0.0150)}
            };

        auto twd_index = ext::make_shared<IborIndex>(Euribor3M());
        std::vector<ext::shared_ptr<RateHelper> > twdInstruments;
        
        for (const auto& q : longtwdQuotes)
        {
            Handle<Quote> quoteHandle(q.second);
            //auto quote = q.second;
            auto helper = ext::make_shared<SwapRateHelper>
            (quoteHandle ,q.first, Taiwan(), Quarterly, ModifiedFollowing, Actual365Fixed(),  twd_index,Handle<Quote>(), 0 * Days);
            twdInstruments.push_back(helper);
        }
        
        auto twdTermStructure = ext::make_shared<PiecewiseYieldCurve<Discount, LogLinear>>(settlementDate, twdInstruments, Actual365Fixed());
        twdTermStructure->enableExtrapolation();
        
        RelinkableHandle<YieldTermStructure> twd_TermStructure;
        twd_TermStructure.linkTo(twdTermStructure);
  
        RelinkableHandle<YieldTermStructure> sofr_TermStructure;
        sofr_TermStructure.linkTo(oisSofrTermStructure);
        
        
        xccyCurveBuilder crsbuilder(Date(30, Aug, 2023),
                                    Taiwan(), 2, ModifiedFollowing,
                                    Actual365Fixed());
        // if TWD = EUR... /
        ext::shared_ptr<IborIndex> baseCcyIdx(new Euribor3M(twd_TermStructure));
        ext::shared_ptr<IborIndex> quoteCcyIdx (new Sofr(sofr_TermStructure));
        
        std::vector<xccyDatum> basisData;
        basisData.emplace_back(1, Years, -10.0);
        basisData.emplace_back(2, Years, -10.0);
        basisData.emplace_back(3, Years, -15.0);
        basisData.emplace_back(4, Years, -20.0);
        basisData.emplace_back(5, Years, -25.0);
        
        
        RelinkableHandle <YieldTermStructure> crsCurve = crsbuilder.xccyCurve(basisData, twd_TermStructure, sofr_TermStructure,
                  baseCcyIdx, quoteCcyIdx);

        
        std::cout << twd_TermStructure->discount(1, true) << std::endl;
        std::cout << twd_TermStructure->discount(2, true) << std::endl;
        std::cout << twd_TermStructure->discount(3, true) << std::endl;
        std::cout << twd_TermStructure->discount(4, true) << std::endl;
        std::cout << twd_TermStructure->discount(5, true) << std::endl;
        
        std::cout << std::endl;
        std::cout << crsCurve->discount(1, true) << std::endl;
        std::cout << crsCurve->discount(2, true) << std::endl;
        std::cout << crsCurve->discount(3, true) << std::endl;
        std::cout << crsCurve->discount(4, true) << std::endl;
        std::cout << crsCurve->discount(5, true) << std::endl;
        
        
        
    

    }
    
    
    catch(std::exception& e)
    {
        std::cout << "EXCEPTION ! " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
