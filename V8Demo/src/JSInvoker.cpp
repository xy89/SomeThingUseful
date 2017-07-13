//
// Created by xy on 16-11-14.
//

#include "JSInvoker.h"
#include "../../../plugin/manager/PluginManager.h"

using namespace demo;

JSInvoker::JSInvoker(Processor processor) : Invoker(processor) {

}

std::string JSInvoker::invoke(std::string &func, std::vector<std::string> &params, TaskContext *taskContext) {
    int argc = 0;
    std::string * args = NULL;
    if (!params.empty()) {
        argc = params.size();
        args = &params[0];
    }

    return PluginManager::getInstance().callJSFunction(source, func, args, argc, taskContext);
}
