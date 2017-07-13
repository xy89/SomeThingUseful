//
// Created by xy on 16-11-3.
//

#include <string.h>
#include "PluginManager.h"
#include "../../../test/plugintest/utils/FileHelper.h"

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "PluginManager"

using namespace demo;

const std::string PluginManager::INVALID = "_invalid_";

class ArrayBufferAllocator : public v8::ArrayBuffer::Allocator {
public:
    virtual void *Allocate(size_t length) {
        void *data = AllocateUninitialized(length);
        return data == NULL ? data : memset(data, 0, length);
    }

    virtual void *AllocateUninitialized(size_t length) { return malloc(length); }

    virtual void Free(void *data, size_t) { free(data); }
};


PluginManager::PluginManager() {
/*    //Initialize V8
    mPlatform = v8::platform::CreateDefaultPlatform();
    v8::V8::InitializePlatform(mPlatform);
    v8::V8::Initialize();
    //Create a new Isolate
    mAllocator = new ArrayBufferAllocator();
    v8::Isolate::CreateParams create_params;
    create_params.array_buffer_allocator = mAllocator;
    mIsolate = v8::Isolate::New(create_params);*/
    mTaskContextWrapper = new TaskContextWrapper();
    mMapWrapper = new MapWrapper();
    mContextMap = new std::map<std::string, v8::Persistent<v8::Context> *>();
}

PluginManager &PluginManager::getInstance() {
    static PluginManager instance;
    return instance;
}

void PluginManager::makeContexts(v8::Isolate *isolate, v8::Local<v8::Object> sandbox, std::string handlers) {
            LOGD("makeContexts\n");
    mIsolate = isolate;
    mSandbox = new v8::Persistent<v8::Object>(mIsolate, sandbox);
    std::map<std::string, std::string> *handlersMap = parseHanderlsStr(handlers);

    if (handlersMap != nullptr && handlersMap->size() > 0) {
        std::map<std::string, std::string>::iterator iter = handlersMap->begin();
        for (; iter != handlersMap->end(); iter++) {
            v8::Local<v8::Context> context = createV8Context();
            v8::Isolate::Scope isolate_scope(mIsolate);
            v8::HandleScope handle_scope(mIsolate);
            v8::Context::Scope context_scope(context);
            //Compile and run the script
            if (executeScript(iter->second)) {
                v8::Persistent<v8::Context> *persistentContext = new v8::Persistent<v8::Context>(mIsolate, context);
                mContextMap->insert(
                        std::pair<std::string, v8::Persistent<v8::Context> *>(iter->first, persistentContext));
            }
        }
    }
}


v8::Local<v8::Context> PluginManager::createV8Context() {
    v8::EscapableHandleScope scope(mIsolate);
    v8::Local<v8::FunctionTemplate> functionTemplate = v8::FunctionTemplate::New(mIsolate);
    v8::Local<v8::Object> sandbox = mSandbox->Get(mIsolate);
    functionTemplate->SetClassName(sandbox->GetConstructorName());

    v8::Local<v8::ObjectTemplate> objectTemplate = functionTemplate->InstanceTemplate();
    v8::NamedPropertyHandlerConfiguration config(GlobalPropertyGetterCallback,0,0,0,0,sandbox);
    objectTemplate->SetHandler(config);

    v8::Local<v8::Context> ctx = v8::Context::New(mIsolate, nullptr, objectTemplate);
    ctx->SetSecurityToken(mIsolate->GetCurrentContext()->GetSecurityToken());
    return scope.Escape(ctx);
}


void PluginManager::GlobalPropertyGetterCallback(v8::Local<v8::Name> property,
                                                 const v8::PropertyCallbackInfo<v8::Value> &args) {
    v8::Local<v8::Object> sandbox = args.Data().As<v8::Object>();
    v8::Isolate* isolate = args.GetIsolate();
    v8::MaybeLocal<v8::Value> maybeResult = sandbox->GetRealNamedProperty(isolate->GetCurrentContext(), property);
    v8::Local<v8::Value> rv;
    if (maybeResult.ToLocal(&rv)) {
        args.GetReturnValue().Set(rv);
    } else {
                //LOGD("GlobalPropertyGetterCallback fails property");
    }
}

std::map<std::string, std::string> *PluginManager::parseHanderlsStr(std::string handlersStr) {
            LOGD("parseHanderlsStr");
    Json::Reader reader;
    Json::Value root;
    std::map<std::string, std::string> *map = new std::map<std::string, std::string>();
    if (reader.parse(handlersStr, root)) {
        for (unsigned i = 0; i < root["handlers"].size(); i++) {
            std::string name = root["handlers"][i]["name"].asString();
            std::string content = root["handlers"][i]["content"].asString();;
                    LOGD("name: %s content: %s\n", name.c_str(), content.c_str());
            map->insert(std::pair<std::string, std::string>(name, content));
        }
    } else {
                LOGD("parseHanderlsStr error");
    }
    return map;
}


std::string PluginManager::callJSFunction(std::string processorName, std::string funcName,
                                          std::string *argv,
                                          int argc, TaskContext *taskContext) {
            LOGD("CallJSFunction with processorName: %s, funName: %s\n", processorName.c_str(), funcName.c_str());
    v8::Isolate::Scope isolate_scope(mIsolate);
    //Create a handle scope to hold the temporary references.
    v8::HandleScope handle_scope(mIsolate);
    std::map<std::string, v8::Persistent<v8::Context> *>::iterator iter = mContextMap->find(processorName);
    if (iter == mContextMap->end()) {
                LOGE("doesn't has a processor with name %s\n", processorName.c_str());
        return INVALID;
    }
    v8::Persistent<v8::Context> *persistentContext = iter->second;
    v8::Local<v8::Context> context = persistentContext->Get(mIsolate);
    //Enter the new context so all the following operations take place within it
    v8::Context::Scope context_scope(context);
    v8::Local<v8::Function> func;
    if (!getJsFunctionAccordingName(funcName).ToLocal(&func)) {
        return INVALID;
    }

    v8::Local<v8::Object> taskContextObj = mTaskContextWrapper->wrapTaskContext(mIsolate, taskContext);

    //Set up an exception handler before calling the function
    v8::TryCatch try_catch(mIsolate);

    v8::Local<v8::Value> args[argc];

    for (int i = 0; i < argc; i++) {
                LOGD("argv: %s\n", argv[i].c_str());
        buildStringArguments(argv[i], args, i);
    }

    v8::Local<v8::Object> globalObj = context->Global();
    globalObj->Set(v8::String::NewFromUtf8(mIsolate, "gTaskContext", v8::NewStringType::kNormal).ToLocalChecked(),
                   taskContextObj);

    v8::Local<v8::Value> result;
    if (!func->Call(context, context->Global(), argc, args).ToLocal(&result)) {
        v8::String::Utf8Value error(try_catch.Exception());
                LOGE("call function error %s\n", *error);
        return INVALID;
    }

    v8::String::Utf8Value utf8Result(result);
    return std::string(*utf8Result);
}

bool PluginManager::setData(std::string processorName, std::map<std::string, std::string> *map) {
            LOGD("setData with processorName: %s\n", processorName.c_str());
    v8::Isolate::Scope isolate_scope(mIsolate);
    //Create a handle scope to hold the temporary references.
    v8::HandleScope handle_scope(mIsolate);
    std::map<std::string, v8::Persistent<v8::Context> *>::iterator iter = mContextMap->find(processorName);
    if (iter == mContextMap->end()) {
                LOGE("doesn't has a processor with name %s\n", processorName.c_str());
        return false;
    }
    v8::Persistent<v8::Context> *persistentContext = iter->second;
    v8::Local<v8::Context> context = persistentContext->Get(mIsolate);
    //Enter the new context so all the following operations take place within it
    v8::Context::Scope context_scope(context);

    v8::Local<v8::Function> func;
    if (!getJsFunctionAccordingName("setData").ToLocal(&func)) {
        return false;
    }

    v8::Local<v8::Object> mapObj = mMapWrapper->wrapMap(mIsolate, map);
    //Set up an exception handler before calling the function
    v8::TryCatch try_catch(mIsolate);
    const int argc = 1;
    v8::Local<v8::Value> argv[argc] = {mapObj};
    v8::Local<v8::Value> result;
    if (!func->Call(context, context->Global(), argc, argv).ToLocal(&result)) {
        v8::String::Utf8Value error(try_catch.Exception());
                LOGE("call function error %s\n", *error);
        return false;
    }
    return true;
}

void PluginManager::buildStringArguments(std::string str, v8::Local<v8::Value> *argList, unsigned int argPos) {
    argList[argPos] = v8::String::NewFromUtf8(mIsolate, str.c_str(), v8::NewStringType::kNormal).ToLocalChecked();
}


v8::MaybeLocal<v8::Function> PluginManager::getJsFunctionAccordingName( std::string funcName) {
    //The script compiled and ran correctly.Now fetch out the function from the global object
    v8::Local<v8::String> func_name = v8::String::NewFromUtf8(mIsolate, funcName.c_str(),
                                                              v8::NewStringType::kNormal).ToLocalChecked();
    v8::Local<v8::Value> func_val;
    //If there is no Process function, or if it is not a function
    if (!mIsolate->GetCurrentContext()->Global()->Get(mIsolate->GetCurrentContext(), func_name).ToLocal(&func_val) ||
        !func_val->IsFunction()) {
                LOGE("can't find function with name  %s\n", funcName.c_str());
        return v8::MaybeLocal<v8::Function>();;
    }
    //It is a function; cast it to a function
    v8::MaybeLocal<v8::Function> func = v8::Local<v8::Function>::Cast(func_val);
    return func;
}


bool PluginManager::executeScript(std::string &script) {
            LOGD("executeScript script: %s\n", script.c_str());
    v8::Local<v8::String> originScript = v8::String::NewFromUtf8(mIsolate, script.c_str(), v8::String::kNormalString,
                                                                 static_cast<int>(script.length()));
    v8::HandleScope handle_scope(mIsolate);
    v8::TryCatch try_catch(mIsolate);
    v8::Local<v8::Context> context(mIsolate->GetCurrentContext());
    //Compile the script and check for errors
    v8::Local<v8::Script> compiled_script;
    if (!v8::Script::Compile(context, originScript).ToLocal(&compiled_script)) {
        v8::String::Utf8Value error(try_catch.Exception());
                LOGE("executeScript compile error: %s\n", *error);
        return false;
    }
    //Run the script
    v8::Local<v8::Value> result;
    if (!compiled_script->Run(context).ToLocal(&result)) {
        v8::String::Utf8Value error(try_catch.Exception());
                LOGE("executeScript run error: %s\n", *error);
        return false;
    }
    return true;
}


PluginManager::~PluginManager() {
            LOGD("~PluginManager");
    std::map<std::string, v8::Persistent<v8::Context> *>::iterator iter = mContextMap->begin();
    for (; iter != mContextMap->end(); iter++) {
        iter->second->Reset();
        delete iter->second;
    }
    mContextMap->clear();
    delete mTaskContextWrapper;
    delete mContextMap;
    delete mMapWrapper;
    mSandbox->Reset();
    delete mSandbox;
    //mIsolate->Dispose();
    //v8::V8::Dispose();
    //v8::V8::ShutdownPlatform();
    //delete mPlatform;
    //delete mAllocator;
}

class ContextifyContext {

};
