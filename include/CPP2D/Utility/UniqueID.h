#pragma once

#include "Timer.h"

namespace cpp2d
{
namespace Utility
{
    class UniqueID
    {
        uint32_t ids[4];

    public:
        UniqueID();

        bool operator==(const UniqueID& id) const;
    };
}
}