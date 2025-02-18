#pragma once

#include <dirent.h>
#include <stdio.h>
#include <string>
#include <vector>

namespace BR {

class Directory {
  public:
    DIR *dir;

    Directory(const char *path) {
        dir = opendir(path);

        if (!dir) {
            printf("Cannot open %s\n", path);
            throw -1;
        }
    }

    dirent *getNextRegular() const {
        while (1) {
            dirent *p = readdir(dir);

            if (!p)
                return 0;

            if (p->d_type == DT_REG)
                return p;
        }
    }

    ~Directory() { closedir(dir); }
};

class FileListing {
  public:
    std::string dirName;
    std::vector<std::string> filenames;

    FileListing(const char *dirName_) : dirName(dirName_) { refresh(); }

    void refresh() {
        Directory directory(dirName.c_str());

        while (1) {
            dirent *entry = directory.getNextRegular();

            if (!entry)
                break;

            filenames.push_back(entry->d_name);
        }
    }
};

} // namespace BR
