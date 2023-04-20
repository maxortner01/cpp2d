#include <CPP2D/Systems.h>

namespace cpp2d
{
namespace Systems
{
    void Animation::update()
    {
        uint32_t counter = 0;
        std::vector<Interface::AnimationNode*> not_finished;
        not_finished.reserve(_nodes.size());
        for (Interface::AnimationNode* node : _nodes)
        {
            node->update();
            if (node->isDone()) 
            {
                if (node->getNextAnimation())
                    not_finished.push_back(node->getNextAnimation());
                
                delete node;
            }
            else 
                not_finished.push_back(node);
        }

        _nodes.shrink_to_fit();
        if (not_finished.size() != _nodes.size())
        {
            _nodes.resize(not_finished.size());
            std::memcpy(&_nodes[0], &not_finished[0], sizeof(void*) * not_finished.size());
        }
    }

    uint32_t Animation::runningAnimations() const
    {
        return _nodes.size();
    }
}
}