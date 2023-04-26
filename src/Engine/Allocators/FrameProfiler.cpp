#include <cpp2d/Engine.h>

#include <cstdlib>

namespace cpp2d::Allocators
{
    FrameProfiler::FrameProfiler() :
        _allocations(0),
        _avg_lifetime(0),
        _frame_counter(0)
    {   }

    void* FrameProfiler::allocate(size_t byteSize)
    {
        void* ptr = std::malloc(byteSize);

        allocations[(_allocations++) % 500] = {
            ptr,
            _lifetime_timer.getTime(),
            0.0,
            byteSize
        };

        return ptr;
    }

    void FrameProfiler::free(void* ptr)
    {
        for (int i = 0; i < 500; i++)
            if (allocations[i].ptr == ptr)
            {
                allocations[i].freeTime = _lifetime_timer.getTime();
                break;
            }

        std::free(ptr);
    }

    void FrameProfiler::endFrame()
    {
        uint32_t freed_ptrs = 0;
        double sum = 0.0;
        for (int i = 0; i < _allocations; i++)
            if (allocations[i].freeTime > 0.0)
            {
                sum += allocations[i].freeTime - allocations[i].allocationTime;
                freed_ptrs++;
            }

        const double avg = sum / (double)freed_ptrs;
        _avg_lifetime = avg;

        std::memset(allocations, 0, sizeof(Allocation) * _allocations_count);
        _allocations = 0;

        _frame_times[(_frame_counter++) % _frame_time_count] = (U32)(1.0 / _frame_timer.getTime());
        _frame_timer.restart();
    }

    U32 FrameProfiler::getFrameRate() const
    {
        R32 sum = 0;
        for (int i = 0; i < _frame_time_count; i++)
            sum += (R32)_frame_times[i];
        
        return (U32)(sum / (R32)_frame_time_count);
    }

    U32 FrameProfiler::getFrameAllocations() const
    {  return _allocations;  }

    double FrameProfiler::getAverageLifetime() const
    {  return _avg_lifetime;  }
}