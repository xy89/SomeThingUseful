#include<node.h>
#include <env.h>
#include "../manager/PluginManager.h"

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "PluginAddon"

namespace  yunos {

    static void makeContexts(const v8::FunctionCallbackInfo<v8::Value>& args){
        if(!args[0]->IsObject()){
            LOGD("sandbox argument must be an object.");
            return;
        }
        v8::Local<v8::Object> sandbox = args[0].As<v8::Object>();
        v8::String::Utf8Value handlers(args[1]);
        std::string handlersStr = std::string(*handlers);
        v8::Isolate* isolate = args.GetIsolate();
        PluginManager::getInstance().makeContexts(isolate,sandbox,handlersStr);

    }

    void callJSFunction(const v8::FunctionCallbackInfo<v8::Value>& args){
        v8::Isolate* isolate = args.GetIsolate();
        v8::String::Utf8Value handlerName(args[0]);
        std::string handlerNameStr = std::string(*handlerName);
        v8::String::Utf8Value funcName(args[1]);
        std::string funcNameStr = std::string(*funcName);
        std::string result = PluginManager::getInstance().callJSFunction(handlerNameStr, funcNameStr, nullptr,0, nullptr);
        args.GetReturnValue().Set(v8::String::NewFromUtf8(isolate, result.c_str()));
    }

    void Init(v8::Local<v8::Object> exports) {
        NODE_SET_METHOD(exports, "makeContexts",makeContexts);
        NODE_SET_METHOD(exports, "callJSFunction",callJSFunction);
    }

    NODE_MODULE(plugin,Init);
}
