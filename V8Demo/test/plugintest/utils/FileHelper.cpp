//
// Created by xy on 16-11-7.
//
#include <fstream>
#include "FileHelper.h"
#include "XiaoYunLogger.h"

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "FileHelper"

using namespace demo;

std::map<std::string, std::string> *FileHelper::getProcessorMap(std::string &configFilePath) {
    LOGD("getProcessorMap %s", configFilePath.c_str());
    std::map<std::string, std::string> *map = new std::map<std::string, std::string>();

    Json::Reader reader;
    Json::Value root;
    std::ifstream is;

    is.open(configFilePath, std::ios::binary);
    if (reader.parse(is, root)) {
        std::string dir = getDirPath(configFilePath);
        for (unsigned i = 0; i < root["processors"].size(); i++) {
            std::string name = root["processors"][i]["processor"].asString();
            std::string path = dir + name;
            std::string source = readJSFile(path);
            LOGD("name: %s path: %s source: %s", name.c_str(), path.c_str(), source.c_str());
            map->insert(std::pair<std::string, std::string>(name, source));
        }
    } else {
        LOGE("readJSONFile parse fail");
    }

    is.close();

    return map;
}

std::string FileHelper::readJSFile(const std::string &name) {
    LOGD("readJSFile name: %s\n", name.c_str());
    FILE *file = fopen(name.c_str(), "rb");
    if (file == NULL) return "";
    //Set the pointer to the end of the file
    fseek(file, 0, SEEK_END);
    //Get the size of file
    size_t size = ftell(file);
    //Rewind the pointer to the begin of the steam
    rewind(file);
    //Set up and read into the buffer
    char *chars = new char[size + 1];
    chars[size] = '\0';
    for (size_t i = 0; i < size;) {
        i += fread(&chars[i], 1, size - i, file);
        if (ferror(file)) {
            fclose(file);
            LOGE("Error reading %s:\n", name.c_str());
            return "";
        }
    }
    //Close file
    fclose(file);
    LOGD("readJSFile: %s\n", chars);
    std::string result(chars);
    delete[] chars;
    return result;
}


std::string FileHelper::getDirPath(std::string &configFilePath) {
    int pos = configFilePath.find_last_of('/');
    std::string dir(configFilePath.substr(0, pos + 1));
    LOGD("dir: %s", dir.c_str());
    return dir;
}
