#pragma once

#include <stdio.h>
#include <ql/quantlib.hpp>

//#include "taibor.hpp"

using namespace QuantLib;

struct xccyDatum
{
    Integer n;
    TimeUnit units;
    Spread basis;

    xccyDatum(Integer n, TimeUnit units, Spread basis) :n(n),
        units(units), basis(basis) {}
};

struct xccyLocalBuilder
{
    // basic datum //
    Real basisPoint;
    Real fxSpot;

    Date today, settlement;
    Calendar calendar;
    Natural settlementDays;
    Currency ccy;
    BusinessDayConvention buisnessConvention;
    DayCounter dayCount;
    bool endOfMonth;

    // curve datum //
    ext::shared_ptr<IborIndex> baseCcyIdx;
    ext::shared_ptr<IborIndex> quoteCcyIdx;


    RelinkableHandle<YieldTermStructure> baseCcyIdxHandle;
    RelinkableHandle<YieldTermStructure> quoteCcyIdxHandle;

    std::vector<xccyDatum> basisData;
};

class xccyCurveBuilder
{
public:
    xccyCurveBuilder(Date today, Calendar calendar,
        Natural settlementDays,
        BusinessDayConvention businessConvention,
        DayCounter dayCount)
    {
        m_today = today;
        m_calendar = calendar;
        m_settlementDays = settlementDays;
        m_businessConvention = businessConvention;
        m_dayCount = dayCount;
    }


    ext::shared_ptr<RateHelper> getCcyRateHelper(const xccyDatum& xccyData,
        RelinkableHandle<YieldTermStructure> baseCcyIdxHandle,
        RelinkableHandle<YieldTermStructure> quoteCcyIdxHandle,

        ext::shared_ptr<IborIndex> baseCcyIdx,
        ext::shared_ptr<IborIndex> quoteCcyIdx,
        bool isFxBaseCurrencyCollateralCurrency = false,
        bool isBasisOnFxBaseCurrencyLeg = true
    );



    RelinkableHandle <YieldTermStructure> xccyCurve(
        std::vector<xccyDatum>& xccyData,

        RelinkableHandle<YieldTermStructure> baseCcyIdxHandle,
        RelinkableHandle<YieldTermStructure> quoteCcyIdxHandle,

        ext::shared_ptr<IborIndex> baseCcyIdx,
        ext::shared_ptr<IborIndex> quoteCcyIdx,
        bool isFxBaseCurrencyCollateralCurrency = false,
        bool isBasisOnFxBaseCurrencyLeg = true);

private:
    Date m_today;
    Calendar m_calendar;
    Natural m_settlementDays;
    BusinessDayConvention m_businessConvention;
    DayCounter m_dayCount;
};
