#pragma once

namespace cpp2d::Utility
{
    template<typename T>
    class State
    {
        T _current_state;

    protected:
        void setState(const T& newState);
        virtual void onStateChange(const T& newState) {}

    public:
        State(const T& initialState);

        T getState() const;
    };
    
    // implementation

    template<typename T>
    void State<T>::setState(const T& newState)
    {
        _current_state = newState;
    }
    
    template<typename T>
    State<T>::State(const T& initialState) :
        _current_state(initialState)
    {   }

    template<typename T>
    T State<T>::getState() const
    {
        return _current_state;
    }
}