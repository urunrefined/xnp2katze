#include "SignalFD.h"

#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <signal.h>
#include <sys/signalfd.h>
#include <unistd.h>

namespace BR {

SignalBlock::SignalBlock(bool unblockOnDestruction_)
    : unblockOnDestruction(unblockOnDestruction_) {
    sigemptyset(&mask);
    sigaddset(&mask, SIGINT);
    sigaddset(&mask, SIGQUIT);

    if (sigprocmask(SIG_BLOCK, &mask, NULL) == -1) {
        throw "sigprocmask failed";
    }
};

SignalBlock::~SignalBlock() {
    if (unblockOnDestruction) {
        sigprocmask(SIG_UNBLOCK, &mask, NULL);
    }
};

SignalFD::SignalFD() : block(false) {
    sfd = signalfd(-1, &(block.mask), O_CLOEXEC);

    if (sfd == -1) {
        throw "signalfd failed";
    }
}

bool SignalFD::isTriggered() {
    pollfd pfd = {sfd, POLL_IN, 0};
    return poll(&pfd, 1, 0) != 0;
}

SignalFD::~SignalFD() { close(sfd); }

} // namespace BR
