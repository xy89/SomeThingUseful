//
// Created by xy on 16-11-3.
//

#ifndef PLUGINMANAGER_H
#define PLUGINMANAGER_H

#include<include/v8.h>
#include<map>
#include <include/libplatform/libplatform.h>
#include "../wrapper/TaskContextWrapper.h"
#include "../../TaskContext.h"
#include "../wrapper/MapWrapper.h"


namespace demo {

    class PluginManager {


    public:
        static PluginManager &getInstance();

        void makeContexts(v8::Isolate* isolate, v8::Local<v8::Object> sandbox, std::string  handlers);

        std::string callJSFunction(std::string processorName, std::string funcName, std::string *args, int argc,
                                   TaskContext *taskContext);

        bool setData(std::string
                     processorName,
                     std::map<std::string, std::string> *map
        );

        ~PluginManager();

        static const std::string INVALID;


    private:

        PluginManager();

        PluginManager(const PluginManager &);

        PluginManager &operator=(const PluginManager &);

        v8::MaybeLocal<v8::Function> getJsFunctionAccordingName(std::string funcName);

        v8::Local<v8::Context> createV8Context();

        static void GlobalPropertyGetterCallback(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);

        bool executeScript(std::string &script);

        std::map<std::string,std::string>* parseHanderlsStr(std::string handlersStr);

        void buildStringArguments(std::string str, v8::Local<v8::Value> *argList, unsigned int argPos);

        v8::Platform *mPlatform;
        v8::Isolate *mIsolate;
        v8::ArrayBuffer::Allocator *mAllocator;
        TaskContextWrapper *mTaskContextWrapper;
        MapWrapper *mMapWrapper;
        std::map<std::string, v8::Persistent<v8::Context> *> *mContextMap;
        v8::Persistent<v8::Object>* mSandbox;

    };
}

#endif //PLUGINMANAGER_H
