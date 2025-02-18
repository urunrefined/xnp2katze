#pragma once

#include <ini.h>
#include <np2.h>
#include <pccore.h>
#include <vector>

class IniCfg {
  public:
    std::vector<INITBL> config;

    IniCfg(NP2OSCFG &np2oscfg, tagNP2Config &np2cfg);
};
