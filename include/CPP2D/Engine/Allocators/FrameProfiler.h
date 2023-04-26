#pragma once

#include "../../Utility.h"

namespace cpp2d::Allocators
{
    struct Allocation
    {
        void* ptr;
        double allocationTime;
        double freeTime;
        size_t byteSize;
    };

    class FrameProfiler :
        public Utility::Singleton<FrameProfiler>
    {
        R64 _avg_lifetime;
        size_t _allocations;

        Timer _frame_timer;
        Timer _lifetime_timer;

        inline static CU32 _frame_time_count = 20;
        U32 _frame_times[_frame_time_count];
        U32 _frame_counter;

        inline static CU32 _allocations_count = 500;
        Allocation allocations[_allocations_count];

    public:
        FrameProfiler();

        void* allocate(size_t byteSize);
        void  free(void* ptr);

        void endFrame();

        U32 getFrameRate() const;
        U32 getFrameAllocations() const;
        R64 getAverageLifetime() const;

        void* operator new(size_t size)
        {
            return std::malloc(size);
        }

        void operator delete(void * p)
        {
            std::free(p);
        }
    };
}