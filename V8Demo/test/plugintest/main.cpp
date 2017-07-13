//
// Created by xy on 16-11-9.
//

#include <iostream>
#include <map>
#include "../../src/plugin/manager/PluginManager.h"
#include "utils/FileHelper.h"
#include "../../JSInvoker.h"


#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "main"

using namespace demo;

int main() {
    std::cout << "Hello, World!" << std::endl;
    std::string configPath = "/system/plugin/config.json";
    std::map<std::string, std::string> *processerMap = FileHelper::getProcessorMap(configPath);
    LOGD("getProcessorMap success");
    //clock_t start_time=clock();
    PluginManager::getInstance().loadConfiguration(processerMap);
    LOGD("loadConfiguration success");
    //test 1
    LOGD("create a processor");
    Processor processor;
    processor.source = "map.js";
    LOGD("create a js invoke");
    JSInvoker jsInvoker(processor);
    LOGD("create a taskcontext");
    TaskContext *taskContext = new TaskContext();
    std::string step("one");
    LOGD("cout step");
    taskContext->countStep(step);
    LOGD("create a vector");
    std::vector<std::string> vector;
    vector.push_back("one");
    std::string funcName = "getValue";
    LOGD("invoke function");
    std::string value = jsInvoker.invoke(funcName, vector, taskContext);
    LOGD("getValue() returned: %s\n", value.c_str());

    //test 2
    Processor processor2;
    processor2.source = "phone.js";
    JSInvoker jsInvoker2(processor2);
    TaskContext *taskContext2 = new TaskContext();
    std::string step2("two");
    taskContext2->countStep(step2);
    taskContext2->countStep(step2);
    std::vector<std::string> vector2;
    vector2.push_back("two");
    std::string funcName2 = "getValue";
    std::string value2 = jsInvoker2.invoke(funcName2, vector2, taskContext2);
    LOGD("getValue() returned: %s\n", value2.c_str());

    std::vector<std::string> vector3;
    vector3.push_back("5");
    std::string funcName3 = "setValue";
    jsInvoker2.invoke(funcName3, vector3, taskContext2);

    std::vector<std::string> vector4;
    std::string funcName4 = "getValue2";
    std::string value3 = jsInvoker2.invoke(funcName4, vector4, taskContext2);
    LOGD("getValue2() returned: %s\n", value3.c_str());

    std::string name = "phone.js";
    std::map<std::string, std::string> map;
    map.insert(std::pair<std::string,std::string>("data", "hello!!!"));
    PluginManager::getInstance().setData(name, &map);


    std::vector<std::string> vector5;
    std::string funcName5 = "getData";
    std::string value4 = jsInvoker2.invoke(funcName5, vector5, taskContext2);
    LOGD("getData() returned: %s\n", value4.c_str());

    delete taskContext;
    delete processerMap;
    return 0;
}
