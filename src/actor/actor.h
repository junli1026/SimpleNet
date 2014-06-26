#ifndef _ACTOR_H_
#define _ACTOR_H_

#include <functional>
#include <memory>
#include "../block.h"

namespace simple{

class Actor{
private:

    std::function<void(std::shared_ptr<Block>)> receiveCallback_;
public:
    void onReceived(std::function<void(std::shared_ptr<Block>)> cb);
    
}

}
#endif
