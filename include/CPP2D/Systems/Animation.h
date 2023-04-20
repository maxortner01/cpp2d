#pragma once

#include <iostream>
#include <vector>
#include "../util.h"
#include "../Utility.h"

namespace cpp2d
{
namespace Systems
{
namespace Interface
{
    struct AnimationNode
    {
        virtual ~AnimationNode() {}
        virtual bool isDone() const = 0;
        virtual void update() = 0;
    };
}

    enum class AnimationState
    {
        Running,
        Done
    };

    template<typename T>
    class AnimationType : 
        public Interface::AnimationNode,
        Utility::State<AnimationState>
    {
        Utility::Timer timer;
        const T _start_value;
        const T _final_value;
        const double _duration;
        const double _delay;
        T* _value;

    public:
        AnimationType(T* value, const T& startValue, const T& finalValue, const double& duration, const double& delay = 0.0) :
            _value(value), 
            _start_value(startValue), 
            _final_value(finalValue), 
            _duration(duration), 
            _delay(delay),
            Utility::State<AnimationState>(AnimationState::Running)
        {   
            *value = startValue;
            timer.restart();
        }

        ~AnimationType()
        {    }

        virtual double interpolate(const double& t) const
        {
            float _t = t / _duration;
            float a = -1.f / (2.f * 0.8f - 1);
            float coeff = a * _t * _t + (1 - a) * _t;

            return coeff * sin(_t * 3.14159 / 2.0);
        }

        void update() override
        {
            const double relative_time = timer.getTime() - _delay;

            if (!isDone() && relative_time > 0) 
            { 
                *_value = _start_value + (_final_value - _start_value) * interpolate(relative_time);

                if (relative_time >= _duration)
                {
                    *_value = _final_value;
                    setState(AnimationState::Done);
                }
            }
        }

        bool isDone() const override
        {
            return (getState() == AnimationState::Done);
        }
    };

    class Animation
    {
        std::vector<Interface::AnimationNode*> _nodes;

    public:
        template<typename T, typename... Args>
        void emplaceAnimation(Args... args)
        {
            _nodes.push_back(new AnimationType<T>(args...));
        }

        void update();
        uint32_t runningAnimations() const;
    };
}
}