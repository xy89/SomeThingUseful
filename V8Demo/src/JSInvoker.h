//
// Created by xy on 16-11-14.
//



#ifndef JSINVOKER_H
#define JSINVOKER_H

#include "Invoker.h"

namespace demo {
        class JSInvoker : public Invoker {
        public:
            JSInvoker(Processor processor);

            virtual std::string
                    invoke(std::string &func, std::vector<std::string> &params, TaskContext *taskContext) override;
        };
};


#endif //JSINVOKER_H
