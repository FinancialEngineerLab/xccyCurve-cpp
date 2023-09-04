/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2007 Ferdinando Ametrano
 Copyright (C) 2007 Chiara Fornarola

 This file is part of QuantLib, a free-software/open-source library
 for financial quantitative analysts and developers - http://quantlib.org/

 QuantLib is free software: you can redistribute it and/or modify it
 under the terms of the QuantLib license.  You should have received a
 copy of the license along with this program; if not, please email
 <quantlib-dev@lists.sf.net>. The license is also available online at
 <http://quantlib.org/license.shtml>.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the license for more details.
*/

#include "krwcd.hpp"
#include <ql/time/calendars/southkorea.hpp>
#include <ql/time/daycounters/actual365fixed.hpp>
#include <ql/currencies/asia.hpp>

namespace QuantLib {

    namespace {

        BusinessDayConvention krwcdConvention(const Period& p) {
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

        bool krwcdEOM(const Period& p) {
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

    KrwCD::KrwCD(const Period& tenor,
                     const Handle<YieldTermStructure>& h)
    : IborIndex("KrwCD", tenor,
                1, // settlement days
                KRWCurrency(), SouthKorea(),
                krwcdConvention(tenor), krwcdEOM(tenor),
                Actual365Fixed(), h) {
        QL_REQUIRE(this->tenor().units()!=Days,
                   "for daily tenors (" << this->tenor() <<
                   ") dedicated DailyTenor constructor must be used");
    }

    KrwCD365::KrwCD365(const Period& tenor,
                           const Handle<YieldTermStructure>& h)
    : IborIndex("KrwCD365", tenor,
                1, // settlement days
				KRWCurrency(), SouthKorea(),
                krwcdConvention(tenor), krwcdEOM(tenor),
                Actual365Fixed(), h) {
        QL_REQUIRE(this->tenor().units()!=Days,
                   "for daily tenors (" << this->tenor() <<
                   ") dedicated DailyTenor constructor must be used");
    }

}