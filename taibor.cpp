#include "taibor.hpp"
#include <ql/time/calendars/taiwan.hpp>
#include <ql/time/daycounters/actual365fixed.hpp>
#include <ql/currencies/asia.hpp>

namespace QuantLib
{

    namespace {

        BusinessDayConvention taiborConvention(const Period& p) {
            switch (p.units()) {
            case Days:
                return ModifiedFollowing;
            case Weeks:
                return Following;
            case Months:
            case Years:
                return ModifiedFollowing;
            default:
                QL_FAIL("invalid time units");
            }
        }

        bool taiborEOM(const Period& p) {
            switch (p.units()) {
            case Days:
            case Weeks:
                return false;
            case Months:
            case Years:
                return true;
            default:
                QL_FAIL("invalid time units");
            }
        }

    }

    Taibor::Taibor(const Period& tenor,
        const Handle<YieldTermStructure>& h)
        : IborIndex("Taibor", tenor,
            1, // settlement days
            TWDCurrency(), Taiwan(),
            taiborConvention(tenor), taiborEOM(tenor),
            Actual365Fixed(), h) {
        QL_REQUIRE(this->tenor().units() != Days,
            "for daily tenors (" << this->tenor() <<
            ") dedicated DailyTenor constructor must be used");
    }

    Taibor365::Taibor365(const Period& tenor,
        const Handle<YieldTermStructure>& h)
        : IborIndex("Taibor365", tenor,
            1, // settlement days
            TWDCurrency(), Taiwan(),
            taiborConvention(tenor), taiborEOM(tenor),
            Actual365Fixed(), h) {
        QL_REQUIRE(this->tenor().units() != Days,
            "for daily tenors (" << this->tenor() <<
            ") dedicated DailyTenor constructor must be used");
    }

}
