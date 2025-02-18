#pragma once

#include <string.h>
#include <string>
#include <vector>

namespace BR {

enum class Count {
    NONE,       /* Flag */
    ONE,        /* Must have exactly one value */
    NONEORMANY, /* May have zero to many values */
    ONEORMANY,  /* Must have at least one value butr may have more */
};

struct Switch {
    const char *name;
    const char shortChar;
    Count count;
    const char *description;
};

class Option {
  public:
    const char *name = 0;
    std::vector<const char *> values;
};

enum class StringType { SHORTNAME, LONGNAME, VALUE };

std::vector<Option> getOptions(int argc, const char *argv[],
                               const std::vector<Switch> &switches);

} // namespace BR
