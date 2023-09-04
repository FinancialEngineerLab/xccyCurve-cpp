#pragma once
#include <ql/indexes/iborindex.hpp>

namespace QuantLib
{
    class Taibor : public IborIndex {
    public:
        Taibor(const Period& tenor,
            const Handle<YieldTermStructure>& h =
            Handle<YieldTermStructure>());
    };

    class Taibor365 : public IborIndex {
    public:
        Taibor365(const Period& tenor,
            const Handle<YieldTermStructure>& h =
            Handle<YieldTermStructure>());
    };

}
