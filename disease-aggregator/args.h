#ifndef ARGS_H
#define ARGS_H

#include "common.h"
#include "external.h"

struct InitialArgs
{
    std::string input_dir;
    int buffer_size, num_workers;
};

InitialArgs ParseInitialArgs(int argc, char *argv[]);

std::tuple<Command, External::Request> ParseCommand(std::string input_string);

#endif
