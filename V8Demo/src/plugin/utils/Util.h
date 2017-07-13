//
// Created by xy on 16-11-15.
//

#ifndef UTIL_H
#define UTIL_H


#include <include/v8.h>

namespace demo {

    class Util {
    public:
        static std::string ObjectToString(v8::Local<v8::Value> value);
    };
}

#endif //UTIL_H
