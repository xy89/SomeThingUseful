//
// Created by xy on 16-11-8.
//
#include "TaskContextWrapper.h"
#include "../utils/Util.h"


#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "TaskContextWrapper"

using namespace demo;

v8::Global<v8::ObjectTemplate> TaskContextWrapper::mTaskContextTemplate;

TaskContextWrapper::TaskContextWrapper() { }

v8::Local<v8::Object> TaskContextWrapper::wrapTaskContext(v8::Isolate *isolate, TaskContext *taskContext) {
    //Local scope for temporary handles.
    v8::EscapableHandleScope handle_scope(isolate);
    if (mTaskContextTemplate.IsEmpty()) {
        v8::Local<v8::ObjectTemplate> raw_template = makeTaskContextTemplate(isolate);
        mTaskContextTemplate.Reset(isolate, raw_template);
    }
    v8::Local<v8::ObjectTemplate> templ = v8::Local<v8::ObjectTemplate>::New(isolate, mTaskContextTemplate);
    v8::Local<v8::Object> obj = templ->NewInstance(isolate->GetCurrentContext()).ToLocalChecked();
    //wrap the raw C++ pointer in an Exteranl so it can be referenced from within JavaScript
    v8::Local<v8::External> taskContextPtr = v8::External::New(isolate, taskContext);
    //Store the taskContext pointer in the JavaScript wrapper
    obj->SetInternalField(0, taskContextPtr);
    return handle_scope.Escape(obj);
}

v8::Local<v8::ObjectTemplate> TaskContextWrapper::makeTaskContextTemplate(v8::Isolate *isolate) {
    LOGD("makeTaskContextTemplate");
    v8::EscapableHandleScope handle_scope(isolate);
    v8::Local<v8::ObjectTemplate> result = v8::ObjectTemplate::New(isolate);
    result->SetInternalFieldCount(1);
    result->Set(v8::String::NewFromUtf8(isolate, "getStepCount", v8::NewStringType::kNormal).ToLocalChecked(),
                v8::FunctionTemplate::New(isolate, getStepCountCallback));
    result->Set(v8::String::NewFromUtf8(isolate, "getTaskSlot", v8::NewStringType::kNormal).ToLocalChecked(),
                v8::FunctionTemplate::New(isolate, getTaskSlotCallback));
    result->Set(v8::String::NewFromUtf8(isolate, "getNluSlot", v8::NewStringType::kNormal).ToLocalChecked(),
                v8::FunctionTemplate::New(isolate, getNluSlotCallback));
    result->Set(v8::String::NewFromUtf8(isolate, "removeSlot", v8::NewStringType::kNormal).ToLocalChecked(),
                v8::FunctionTemplate::New(isolate, removeSlotCallback));
    result->Set(v8::String::NewFromUtf8(isolate, "inheritSlot", v8::NewStringType::kNormal).ToLocalChecked(),
                v8::FunctionTemplate::New(isolate, inheritSlotCallback));
    result->Set(
            v8::String::NewFromUtf8(isolate, "copyActionParamFromSlot", v8::NewStringType::kNormal).ToLocalChecked(),
            v8::FunctionTemplate::New(isolate, copyActionParamFromSlotCallback));

    return handle_scope.Escape(result);
}

void TaskContextWrapper::getStepCountCallback(const v8::FunctionCallbackInfo<v8::Value> &args) {
    LOGD("getStepCountCallback args length: %d\n", args.Length());
    TaskContext *taskContextPtr = unWrapTaskContext(args);
    LOGD("args[0]: %s\n", Util::ObjectToString(args[0]->ToString()).c_str());
    std::string key = Util::ObjectToString(args[0]->ToString());
    double value = taskContextPtr->getStepCount(key);
    args.GetReturnValue().Set(v8::Number::New(args.GetIsolate(), value));
}

void TaskContextWrapper::getTaskSlotCallback(const v8::FunctionCallbackInfo<v8::Value> &args) {
    LOGD("getNluResultCallback args length: %d\n", args.Length());
    TaskContext *taskContextPtr = unWrapTaskContext(args);
    LOGD("args[0]: %s\n", Util::ObjectToString(args[0]->ToString()).c_str());
    std::string key = Util::ObjectToString(args[0]->ToString());
    std::string value = taskContextPtr->getTaskSlot(key);
    args.GetReturnValue().Set(v8::String::NewFromUtf8(args.GetIsolate(), value.c_str(), v8::NewStringType::kNormal,
                                                      static_cast<int>(value.length())).ToLocalChecked());

}

void TaskContextWrapper::getNluSlotCallback(const v8::FunctionCallbackInfo<v8::Value> &args) {
    LOGD("getNluSlotCallback args length: %d\n", args.Length());
    TaskContext *taskContextPtr = unWrapTaskContext(args);
    LOGD("args[0]: %s\n", Util::ObjectToString(args[0]->ToString()).c_str());
    std::string key = Util::ObjectToString(args[0]->ToString());
    std::string value = taskContextPtr->getNluSlot(key);
    args.GetReturnValue().Set(v8::String::NewFromUtf8(args.GetIsolate(), value.c_str(), v8::NewStringType::kNormal,
                                                      static_cast<int>(value.length())).ToLocalChecked());
}

void TaskContextWrapper::removeSlotCallback(const v8::FunctionCallbackInfo<v8::Value> &args) {
    LOGD("removeSlotCallback args length: %d\n", args.Length());
    TaskContext *taskContextPtr = unWrapTaskContext(args);
    LOGD("args[0]: %s\n", Util::ObjectToString(args[0]->ToString()).c_str());
    std::string key = Util::ObjectToString(args[0]->ToString());
    taskContextPtr->removeSlot(key);
}

void TaskContextWrapper::inheritSlotCallback(const v8::FunctionCallbackInfo<v8::Value> &args) {
    LOGD("inheritSlotCallback args length: %d\n", args.Length());
    TaskContext *taskContextPtr = unWrapTaskContext(args);
    LOGD("args[0]: %s\n", Util::ObjectToString(args[0]->ToString()).c_str());
    std::string key = Util::ObjectToString(args[0]->ToString());
    taskContextPtr->inheritSlot(key);
}

void TaskContextWrapper::copyActionParamFromSlotCallback(const v8::FunctionCallbackInfo<v8::Value> &args) {
    LOGD("copyActionParamFromSlotCallback args length: %d\n", args.Length());
    TaskContext *taskContextPtr = unWrapTaskContext(args);
    LOGD("args[0]: %s\n", Util::ObjectToString(args[0]->ToString()).c_str());
    std::string key = Util::ObjectToString(args[0]->ToString());
    taskContextPtr->copyActionParamFromSlot(key);
}

TaskContext *TaskContextWrapper::unWrapTaskContext(const v8::FunctionCallbackInfo<v8::Value> &info) {
    v8::Local<v8::Object> self = info.Holder();
    v8::Local<v8::External> external = v8::Local<v8::External>::Cast(self->GetInternalField(0));
    TaskContext *taskContextPtr = static_cast<TaskContext *>(external->Value());
    return taskContextPtr;
}

TaskContextWrapper::~TaskContextWrapper() {
    mTaskContextTemplate.Reset();
}
