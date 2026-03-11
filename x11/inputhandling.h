#pragma once

#include "glfw/Input.h"
#include "keystat.h"
#include "pulse/PulseSoundEngine.h"
#include "util/Enums.h"

namespace BR {

struct KeyMappingNew {
  public:
    KeyButtons key;
    PC98Key pc98Key;
};

class InputMapper {
    std::vector<KeyMappingNew> mapping;

  public:
    InputMapper(std::vector<KeyMappingNew> mapping_) : mapping(mapping_) {}
    bool handleSpecialInput(Input &input, ViewPortMode &viewPortMode,
                     VisualScreen &visualScreen, DoubleLines &doubleLines,
                     Sfx::PulseSoundEngine &soundRef);

    void handleInputKeys(Input &input);
};

std::vector<KeyMappingNew> getDefaultKeyMap();
std::vector<KeyMappingNew> readKeyMap(const char *filename);
void saveKeyMap(const char *filename,
                const std::vector<KeyMappingNew> &mapping);

} // namespace BR
