#pragma once

namespace cpp2d::Utility
{
    /**
     * @brief A class that prevents copying and assignment.
     * 
     * This class can be used as a base class to prevent the copy constructor and 
     * copy assignment operator from being generated for a derived class.
     */
    class NoCopy
    {
    public:
        NoCopy() = default;
        NoCopy(const NoCopy&) = delete;
        NoCopy& operator=(const NoCopy&) = delete;
    };

}
