//
// Created by xy on 16-11-15.
//

#ifndef MAPWRAPPER_H
#define MAPWRAPPER_H

#include <include/v8.h>

namespace demo {

    class MapWrapper {
    public:
        MapWrapper();

        ~MapWrapper();

        v8::Local<v8::Object> wrapMap(v8::Isolate *isolate, std::map<std::string, std::string> *obj);

    private:

        static v8::Local<v8::ObjectTemplate> makeMapTemplate(v8::Isolate *isolate);

        static std::map<std::string, std::string>* unWrapMap(v8::Local<v8::Object> obj);

        static void mapGet(v8::Local<v8::Name> name, const v8::PropertyCallbackInfo<v8::Value> &info);

        static void mapSet(v8::Local<v8::Name> name, v8::Local<v8::Value> value,
                           const v8::PropertyCallbackInfo<v8::Value> &info);

        static v8::Global<v8::ObjectTemplate> mMapTemplate;


    };
}


#endif //MAPWRAPPER_H
