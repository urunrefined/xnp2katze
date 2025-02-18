#pragma once

#include <signal.h>

namespace BR {

class SignalBlock {
    bool unblockOnDestruction;

  public:
    sigset_t mask;

    SignalBlock(bool unblockOnDestruction_);
    ~SignalBlock();
};

class SignalFD {
    SignalBlock block;
    int sfd;

  public:
    SignalFD();
    bool isTriggered();
    ~SignalFD();
};

} // namespace BR
