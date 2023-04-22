#pragma once

#include <cpp2d/Graphics.h>
#include <iostream>
#include <vector>
#include "../util.h"

namespace cpp2d
{
namespace Engine
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
         * @brief Virtual destructor
         * 
         * This is a virtual destructor to ensure that the correct destructor for the derived class is 
         * called when a pointer to the base class is deleted. 
         * 
         * It also calls the virtual method onDelete in order to pass to the user the ability
         * to mutate the next animation class (for example to add a new animation in the sequence).
         */
        virtual ~AnimationNode() 
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

        virtual bool isWaiting() const = 0;

        /**
         * @brief Update the animation state
         * 
         * This pure virtual function updates the state of the animation for the next frame.
         */
        virtual void update() = 0;

        /**
         * @brief Starts the animation
         * 
         * This pure virtual function starts the animation. This is usually called upon 
         * the parent's destruction in the Animation class.
         */
        virtual void start() = 0;

        virtual void stop() = 0;
    };
}

    enum class AnimationState
    {
        Waiting,
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
        public    Interface::AnimationNode,
        protected Utility::State<AnimationState>
    {
        Timer   timer;
        const T _start_value;
        const T _final_value;
        const double _duration;
        const double _delay;
        T* _value;
        Interface::AnimationNode* _next_node;

    protected:
        T* getValue() const
        {
            return _value;
        }

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
            Utility::State<AnimationState>(AnimationState::Waiting)
        {   }

        /**
         * @brief Virtual destructor
         * 
         * This is a virtual destructor to ensure that the correct destructor for the derived class is 
         * called when a pointer to the base class is deleted.
         */
        virtual ~AnimationType()
        {    }

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

        bool isWaiting() const override
        {
            return (getState() == AnimationState::Waiting);
        }

        double getDelay() const
        {
            return _delay;
        }

        double getDuration() const
        {
            return _duration;
        }

        T getStartValue() const
        {
            return _start_value;
        }

        T getFinalValue() const
        {
            return _final_value;
        }

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

            if (getState() == AnimationState::Running && relative_time > 0) 
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
         * @brief This starts the animation.
         */
        void start() override
        {
            setState(AnimationState::Running);
            timer.restart();
        }

        void stop() override
        {
            setState(AnimationState::Done);
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

    /**
     * @brief The Animation class manages a list of animation nodes and updates them over time
     */
    class Animation
    {
        std::vector<Interface::AnimationNode*> _nodes;

    public:
        /**
         * @brief Add a new animation node to the list of nodes
         *
         * @tparam T The type of animation node to add
         * @tparam Args The types of arguments to pass to the constructor of the new node
         * @param args The arguments to pass to the constructor of the new node
         */
        template<typename T, typename... Args>
        T* emplaceAnimation(Args... args)
        {
            _nodes.push_back(new T(args...));

            _nodes[_nodes.size() - 1]->start();

            return (T*)_nodes[_nodes.size() - 1];
        }

        void pushAnimation(Interface::AnimationNode* node)
        {
            _nodes.push_back(node);
        }

        /**
         * @brief Update all animations in the list of nodes
         */
        void update();

        /**
         * @brief Get the number of running animations
         * 
         * @return uint32_t The number of running animations
         */
        uint32_t runningAnimations() const;
    };
}
}