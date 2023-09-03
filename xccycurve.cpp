#include "xccycurve.hpp"

using namespace QuantLib;

ext::shared_ptr<RateHelper> xccyCurveBuilder::getCcyRateHelper(
                                             const xccyDatum& xccyData,
                                             RelinkableHandle<YieldTermStructure> baseCcyIdxHandle,
                                             RelinkableHandle<YieldTermStructure> quoteCcyIdxHandle,
                                             ext::shared_ptr<IborIndex> baseCcyIdx,
                                             ext::shared_ptr<IborIndex> quoteCcyIdx,
                                             bool isFxBaseCurrencyCollateralCurrency,
                                             bool isBasisOnFxBaseCurrencyLeg
                                                                         )
{
    Handle<Quote> quoteHandle(ext::make_shared<SimpleQuote>( xccyData.basis * 1.0e-4));
    Period tenor(xccyData.n, xccyData.units);
    
    Handle<YieldTermStructure> collateralHandle =
            isFxBaseCurrencyCollateralCurrency ? baseCcyIdxHandle : quoteCcyIdxHandle;
    
    ext::shared_ptr<RateHelper> ccyRateHelper(new ConstNotionalCrossCurrencyBasisSwapRateHelper(quoteHandle, tenor, m_settlementDays, m_calendar, m_businessConvention, true,
        baseCcyIdx, quoteCcyIdx,
        collateralHandle,
        isFxBaseCurrencyCollateralCurrency,
        isBasisOnFxBaseCurrencyLeg));
    
    return ccyRateHelper;
}


RelinkableHandle <YieldTermStructure> xccyCurveBuilder::xccyCurve(std::vector<xccyDatum>& xccyData,
    RelinkableHandle<YieldTermStructure> baseCcyIdxHandle,
    RelinkableHandle<YieldTermStructure> quoteCcyIdxHandle,
    ext::shared_ptr<IborIndex> baseCcyIdx,
    ext::shared_ptr<IborIndex> quoteCcyIdx,
    bool isFxBaseCurrencyCollateralCurrency,
    bool isBasisOnFxBaseCurrencyLeg)
{
    Date settlement = m_calendar.advance(m_today, m_settlementDays * Days);

    // rate helper building ! //
    std::vector<ext::shared_ptr<RateHelper> > instruments;
    
    instruments.reserve(xccyData.size());
    for (const auto& i : xccyData)
    {
        instruments.push_back(getCcyRateHelper(i, baseCcyIdxHandle, quoteCcyIdxHandle,
                                                           baseCcyIdx,
                                                           quoteCcyIdx,
                                                           isFxBaseCurrencyCollateralCurrency,
                                                           isBasisOnFxBaseCurrencyLeg));
    }
    ext::shared_ptr<YieldTermStructure> foreignCcyCurve(
            new PiecewiseYieldCurve<Discount, LogLinear>(settlement, instruments, m_dayCount));
    foreignCcyCurve->enableExtrapolation();
    RelinkableHandle<YieldTermStructure> foreignCurve(foreignCcyCurve);
    
    return foreignCurve;
}
