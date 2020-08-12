#ifndef ARGS_H
#define ARGS_H

#include "common.h"

const std::string NL = "\n";
const std::string TAB = "\t";


struct InitialArgs
{
    std::string inputDir;
    int bufferSize, numWorkers;
};

InitialArgs parseInitialArgs(int argc, char *argv[]);

std::tuple<Command, Request> parseCommand(std::string inputString);

#endif
