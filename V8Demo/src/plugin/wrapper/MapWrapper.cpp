//
// Created by xy on 16-11-15.
//

#include "MapWrapper.h"
#include "../utils/Util.h"


#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "MapWrapper"

using namespace demo;

v8::Global<v8::ObjectTemplate> MapWrapper::mMapTemplate;


MapWrapper::MapWrapper() {
}

v8::Local<v8::Object> MapWrapper::wrapMap(v8::Isolate *isolate, std::map<std::string, std::string> *obj) {
    v8::EscapableHandleScope handle_scope(isolate);
    if (mMapTemplate.IsEmpty()) {
        v8::Local<v8::ObjectTemplate> raw_template = makeMapTemplate(isolate);
        mMapTemplate.Reset(isolate, raw_template);
    }

    v8::Local<v8::ObjectTemplate> templ = v8::Local<v8::ObjectTemplate>::New(isolate, mMapTemplate);
    v8::Local<v8::Object> result = templ->NewInstance(isolate->GetCurrentContext()).ToLocalChecked();
    v8::Local<v8::External> map_ptr = v8::External::New(isolate, obj);
    result->SetInternalField(0, map_ptr);
    return handle_scope.Escape(result);

}

v8::Local<v8::ObjectTemplate> MapWrapper::makeMapTemplate(v8::Isolate *isolate) {
    v8::EscapableHandleScope handle_scope(isolate);
    v8::Local<v8::ObjectTemplate> result = v8::ObjectTemplate::New(isolate);
    result->SetInternalFieldCount(1);
    result->SetHandler(v8::NamedPropertyHandlerConfiguration(mapGet, mapSet));
    return handle_scope.Escape(result);
}

void MapWrapper::mapGet(v8::Local<v8::Name> name, const v8::PropertyCallbackInfo<v8::Value> &info) {
    if (name->IsSymbol()) return;
    std::map<std::string, std::string> *obj = unWrapMap(info.Holder());
    std::string key = Util::ObjectToString(v8::Local<v8::String>::Cast(name));
    std::map<std::string, std::string>::iterator iter = obj->find(key);
    if (iter == obj->end())return;
    const std::string &value = (*iter).second;
    info.GetReturnValue().Set(v8::String::NewFromUtf8(info.GetIsolate(), value.c_str(), v8::NewStringType::kNormal,
                                                      static_cast<int>(value.length())).ToLocalChecked());
}

void MapWrapper::mapSet(v8::Local<v8::Name> name, v8::Local<v8::Value> value_obj,
                        const v8::PropertyCallbackInfo<v8::Value> &info) {
    if (name->IsSymbol()) return;
    std::map<std::string, std::string> *obj = unWrapMap(info.Holder());
    std::string key = Util::ObjectToString(v8::Local<v8::String>::Cast(name));
    std::string value = Util::ObjectToString(value_obj);
    (*obj)[key] = value;
    info.GetReturnValue().Set(value_obj);
}

std::map<std::string, std::string> *MapWrapper::unWrapMap(v8::Local<v8::Object> obj) {
    v8::Local<v8::External> field = v8::Local<v8::External>::Cast(obj->GetInternalField(0));
    void *ptr = field->Value();
    return static_cast<std::map<std::string, std::string> *>(ptr);
}

MapWrapper::~MapWrapper() {
    mMapTemplate.Reset();
}
