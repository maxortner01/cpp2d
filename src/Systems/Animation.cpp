#include <CPP2D/Systems.h>

namespace cpp2d
{
namespace Systems
{
    void Animation::update()
    {
        for (Interface::AnimationNode*& node : _nodes)
        {
            node->update();

            if (node->isDone())
            {
                Interface::AnimationNode* next_node = node->getNextAnimation();
                delete node;
                if (next_node) next_node->start();
                node = next_node;
            }
        }

        std::vector<Interface::AnimationNode*> no_nullptr;
        std::copy_if(_nodes.begin(), _nodes.end(), std::back_inserter(no_nullptr), [](Interface::AnimationNode* a) { return a != nullptr; });
        std::copy(no_nullptr.begin(), no_nullptr.end(), _nodes.begin());
    }

    uint32_t Animation::runningAnimations() const
    {
        return _nodes.size();
    }
}
}