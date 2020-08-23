#ifndef ARGS_H
#define ARGS_H

#include "common.h"
#include "external.h"

struct InitialArgs
{
    std::string inputDir;
    int bufferSize, numWorkers;
};

InitialArgs parseInitialArgs(int argc, char *argv[]);

std::tuple<Command, External::Request> parseCommand(std::string inputString);

#endif
