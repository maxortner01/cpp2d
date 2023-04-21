#pragma once

#include <sstream>
#include <cstring>
#include <cstdlib>
#include "Timer.h"

namespace cpp2d
{
namespace Utility
{
    class UniqueID
    {
        uint32_t ids[4];

    public:
        UniqueID()
        {
            uint32_t offset = 0;
            const Timer timer;
            float time = (float)timer.getTime();

            uint32_t seed;
            std::memcpy(&seed, &time, sizeof(uint32_t));
            srand(seed * seed * seed);
            ids[0] = rand();
            ids[1] = rand();
            ids[2] = rand();

            void* ptr = (void*)this;
            std::memcpy(&ids[3], &ptr, sizeof(uint32_t));
        }

        bool operator==(const UniqueID& id) const
        {
            for (uint32_t i = 0; i < 4; i++)
                if (ids[i] != id.ids[i])
                    return false;

            return true;
        }

        std::string string() const
        {
            std::stringstream stream;
            
            char* c = (char*)ids;
            for (uint32_t i = 0; i < sizeof(ids); i++)
                stream << *(c++);

            return stream.str();
        }
    };
}
}