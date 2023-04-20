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
    /**
     * @class AnimationNode
     * @brief A base class for defining animation nodes that can act as a linked list.
     * This class provides an interface for defining animation nodes that can act as a linked list.
     * The nodes represent individual animations that can be chained together, and each node contains
     * a pointer to the next node in the sequence.
     */
    class AnimationNode
    {
        AnimationNode* _next_node;

    public:
        /**
        * @brief Default constructor
        *
        * Initializes the next node pointer to null.
        */
        AnimationNode() :
            _next_node(nullptr)
        {   }

        /**
         * @brief Set the animation that should be run when this one finishes
         * 
         * This function sets the next animation in the sequence by taking a pointer to an 
         * AnimationNode object. The pointer creation and deletion is handled in the Animation class, 
         * and should not be handled by the user.
         * 
         * @param _node A pointer to the next animation node in the sequence
         */
        void setNextAnimation(AnimationNode* _node)
        {
            _next_node = _node;
        }

        /**
         * @brief Get the next animation node in the sequence
         * 
         * @return A pointer to the next animation node in the sequence
         */
        AnimationNode* getNextAnimation() const
        {
            return _next_node;
        }

        /**
         * @brief Virtual destructor
         * 
         * This is a virtual destructor to ensure that the correct destructor for the derived class is 
         * called when a pointer to the base class is deleted.
         */
        virtual ~AnimationNode() {}

        /**
         * @brief Interpolate the animation at the given time
         * 
         * This pure virtual function calculates the current state of the animation at the given time 
         * by interpolating between the start and end values of the animation.
         * 
         * @param t The current time of the animation
         * @return The interpolated value of the animation at the given time
         */
        virtual double interpolate(const double& t) const = 0;

        /**
         * @brief Check if the animation is complete
         * 
         * This pure virtual function checks whether the animation is complete or not.
         * 
         * @return True if the animation is complete, false otherwise
         */
        virtual bool isDone() const = 0;

        /**
         * @brief Update the animation state
         * 
         * This pure virtual function updates the state of the animation for the next frame.
         */
        virtual void update() = 0;
    };
}

    enum class AnimationState
    {
        Running,
        Done
    };

    /**
     *@class AnimationType
     *@brief A templated class for defining typed animation nodes.
     *This class provides a templated implementation of the AnimationNode interface for defining typed
     *animation nodes. The nodes represent individual animations that can be chained together, and each
     *node contains a pointer to the next node in the sequence.
     *@tparam T The type of value being animated
     */
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
        Interface::AnimationNode* _next_node;

    public:
        /**
         * @brief Constructor
         *
         * This constructor initializes the animation with the given parameters and starts the timer.
         *
         * @param value A pointer to the value being animated
         * @param startValue The starting value of the animation
         * @param finalValue The final value of the animation
         * @param duration The duration of the animation
         * @param delay The delay before the animation begins
         */
        AnimationType(T* value, const T& startValue, const T& finalValue, const double& duration, const double& delay = 0.0) :
            _value(value), 
            _start_value(startValue), 
            _final_value(finalValue), 
            _duration(duration), 
            _delay(delay),
            Utility::State<AnimationState>(AnimationState::Running)
        {   
            timer.restart();
        }

        /**
         * @brief Virtual destructor
         * 
         * This is a virtual destructor to ensure that the correct destructor for the derived class is 
         * called when a pointer to the base class is deleted.
         */
        virtual ~AnimationType()
        {    }

        /**
         * @brief Update the animation state
         * 
         * This function updates the state of the animation for the next frame. It calculates the current 
         * state of the animation using the timer and the duration, and interpolates between the start 
         * and end values of the animation to calculate the current value. 
         * 
         * If the animation is not yet complete and the relative time has exceeded 1, the animation is 
         * marked as done and the final value is set.
         */
        void update() override
        {
            const double relative_time = (timer.getTime() - _delay) / _duration;

            if (!isDone() && relative_time > 0) 
            { 
                *_value = _start_value + (_final_value - _start_value) * interpolate(relative_time);

                if (relative_time >= 1)
                {
                    *_value = _final_value;
                    setState(AnimationState::Done);
                }
            }
        }

        /**
         * @brief Check if the animation is complete
         * 
         * This function checks whether the animation is complete or not by checking the state of the 
         * animation. 
         * 
         * @return True if the animation is complete, false otherwise
         */
        bool isDone() const override
        {
            return (getState() == AnimationState::Done);
        }
    };

    template<typename T>
    class AnimationInterpolate :
        public AnimationType<T>
    {
    public:
        using AnimationType<T>::AnimationType;

        double interpolate(const double& t) const override
        {
            float a = -1.f / (2.f * 0.8f - 1);
            float coeff = a * t * t + (1 - a) * t;

            return coeff * sin(t * 3.14159 / 2.0);
        }
    };

    class Animation
    {
        std::vector<Interface::AnimationNode*> _nodes;

    public:
        template<typename T, typename... Args>
        void emplaceAnimation(Args... args)
        {
            _nodes.push_back(new T(args...));
        }

        void update();
        uint32_t runningAnimations() const;
    };
}
}