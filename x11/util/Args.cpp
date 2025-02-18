#include "Args.h"

namespace BR {

static StringType getStringType(const char *str) {
    if (str[0] == '-') {
        if (str[1] == '-') {
            return StringType::LONGNAME;
        }

        if (strlen(str) == 2) {
            return StringType::SHORTNAME;
        }
    }

    return StringType::VALUE;
}

static const Switch *getSwitch(const char *str,
                               const std::vector<Switch> &switches) {
    StringType type = getStringType(str);

    if (type == StringType::LONGNAME) {
        for (const Switch &sw : switches) {
            if (strcmp(sw.name, str + 2) == 0) {
                return &sw;
            }
        }
    } else if (type == StringType::SHORTNAME) {
        for (const Switch &sw : switches) {
            if (str[1] == sw.shortChar) {
                return &sw;
            }
        }
    }

    return 0;
}

std::vector<Option> getOptions(int argc, const char *argv[],
                               const std::vector<Switch> &switches) {

    std::vector<Option> ret;

    // Remove name
    if (argc) {
        argc--;
        argv++;
    }

    while (argc) {
        const Switch *sw = getSwitch(argv[0], switches);

        if (!sw) {
            throw "Is not a switch";
        }

        argc--;
        argv++;

        // TODO: If option is already present throw an error

        for (auto &option : ret) {
            if (strcmp(option.name, sw->name) == 0) {
                throw "Option specified more than once";
            }
        }

        Option option{sw->name, {}};

        // Get Argument for option
        while (argc) {
            StringType type = getStringType(argv[0]);
            if (type != StringType::VALUE) {
                break;
            }

            option.values.push_back(argv[0]);

            argc--;
            argv++;
        }

        if (sw->count == Count::NONE) {
            if (!option.values.empty()) {

                throw "Did not expect a parameter";
            }
        } else if (sw->count == Count::ONE) {
            if (option.values.size() != 1) {
                throw "Expects one argument";
            }
        } else if (sw->count == Count::NONEORMANY) {

        } else if (sw->count == Count::ONEORMANY) {
            if (option.values.size() < 1) {
                throw "Expects at least one argument";
            }
        }

        ret.push_back(option);
    }

    return ret;
}

} // namespace BR
