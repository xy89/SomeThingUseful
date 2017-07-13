//
// Created by xy on 16-11-8.
//

#ifndef DATAPROVIDERWRAPPER_H
#define DATAPROVIDERWRAPPER_H

#include<include/v8.h>
#include "../../TaskContext.h"


namespace demo {

    class TaskContextWrapper {
    public :
        TaskContextWrapper();

        ~TaskContextWrapper();

        v8::Local<v8::Object> wrapTaskContext(v8::Isolate *isolate, TaskContext *taskContext);

    private:

        static v8::Local<v8::ObjectTemplate> makeTaskContextTemplate(v8::Isolate *isolate);

        static TaskContext *unWrapTaskContext(const v8::FunctionCallbackInfo<v8::Value> &info);

        static void getStepCountCallback(const v8::FunctionCallbackInfo<v8::Value> &args);

        static void getTaskSlotCallback(const v8::FunctionCallbackInfo<v8::Value> &args);

        static void getNluSlotCallback(const v8::FunctionCallbackInfo<v8::Value> &args);

        static void removeSlotCallback(const v8::FunctionCallbackInfo<v8::Value> &args);

        static void inheritSlotCallback(const v8::FunctionCallbackInfo<v8::Value> &args);

        static void copyActionParamFromSlotCallback(const v8::FunctionCallbackInfo<v8::Value> &args);

        static v8::Global<v8::ObjectTemplate> mTaskContextTemplate;
    };

}
#endif //DATAPROVIDERWRAPPER_H
