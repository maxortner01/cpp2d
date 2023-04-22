#include <CPP2D/Utility.h>

#include <cstdlib>
#include <cstring>

namespace cpp2d
{
namespace Utility
{
    UniqueID::UniqueID()
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

    bool UniqueID::operator==(const UniqueID& id) const 
    {
        for (uint32_t i = 0; i < 4; i++)
            if (ids[i] != id.ids[i])
                return false;

        return true;
    }
}
}