#pragma once

#include "inputhandling.h"
#include "np2.h"
#include "pccore.h"
#include "pulse/PulseSoundEngine.h"
#include "util/SignalFD.h"

namespace BR {
void loop(SignalFD &sfd, InputMapper &inputMapper, NP2CFG &cfg, NP2OSCFG &oscfg,
          Sfx::PulseSoundEngine &soundEngine);
}
