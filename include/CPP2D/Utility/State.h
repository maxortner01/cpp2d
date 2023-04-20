#pragma once

namespace cpp2d
{
namespace Utility
{
    template<typename T>
    class State
    {
        T _state;

    protected:
        void setState(const T& state) { onStateChanged(state); _state = state; }

    public:
        State(const T& _initState = (T)0) : _state(_initState) {   }

        T getState() const { return _state; }

        virtual void onStateChanged(const T& newState) {  }
    };
}
}