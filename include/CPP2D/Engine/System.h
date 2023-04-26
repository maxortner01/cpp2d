#pragma once

namespace cpp2d
{
    class Scene;

    // Possibly template this so that the user specifies which components are 
    // attached to this system explicitly
    class System
    {
        std::vector<const System*> _child_systems;

    public:
        friend class Scene;

        System();

        void addChild(const System* const child);
        bool hasChild(const System* const child) const;

        virtual void update(Scene* const scene, double dt) = 0;
    };
}