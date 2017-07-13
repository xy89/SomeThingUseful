//
// Created by xy on 16-11-15.
//

#include "Util.h"

using namespace demo;

/**
 * Convert a JavaScript string to a std::string
 */
std::string Util::ObjectToString(v8::Local<v8::Value> value) {
    v8::String::Utf8Value utf8_value(value);
    return std::string(*utf8_value);
}



