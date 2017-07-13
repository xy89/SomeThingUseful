//
// Created by xy on 16-11-7.
//
#ifndef FILEHELPER_H
#define FILEHELPER_H

#include <iostream>
#include <map>
#include "include/json/json.h"

namespace demo {

    class FileHelper {

    public:
        static std::map<std::string, std::string>* getProcessorMap(std::string &configFilePath);

    private:

        static std::string readJSFile(const std::string &name);

        static std::string getDirPath(std::string &file);
    };
}


#endif //FILEHELPER_H
