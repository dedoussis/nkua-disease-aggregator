
#include <unistd.h>
#include <iostream>
#include <map>
#include "args.h"
#include <vector>
#include <sstream>
#include "common.h"

InitialArgs parseInitialArgs(int argc, char* argv[]) {
    const std::string helpMessage = "Usage: " + std::string(argv[0]) + " <option(s)>" + NL
        + "Options:" + NL
        + TAB + "-w NUMBER_OF_WORKERS" + TAB + "The number of workers to use" + NL
        + TAB + "-b BUFFER_SIZE" + TAB + TAB + "The size of the buffer" + NL
        + TAB + "-i INPUT_DIR" + TAB + TAB + "Directory containing all the input data" + NL
        + TAB + "-h" + TAB + TAB + TAB + "Show this help message";

    if (argc <= 1)
        throw helpMessage;

    InitialArgs args;
    int c;

    while ((c = getopt(argc, argv, "w:b:i:h")) != -1)
    {
        switch (c)
        {
        case 'w':
            args.numWorkers = std::stoi(optarg);
            break;
        case 'b':
            args.bufferSize = std::stoi(optarg);
            break;
        case 'i':
            args.inputDir = optarg;
            break;
        default:
            throw helpMessage;
        }
    }
    return args;
}

std::map<std::string, Command> commandRegistry = {
    {"diseaseFrequency", Command::DiseaseFrequency},
    {"searchPatientRecord", Command::SearchPatientRecord},
    {"exit", Command::Exit},
};

std::tuple<Command, Request> parseCommand(std::string inputString)
{
    std::stringstream inputStream(inputString);
    std::string commandString, rest;
    inputStream >> commandString >> rest;

    if (!inputString.empty() && commandRegistry.contains(commandString))
    {
        Command command = commandRegistry[commandString];
        Deserializer<Request> deserializer = getRequestDeserializer(command);
        return std::make_tuple(command, deserializer(rest));
    }
    throw std::runtime_error("Invalid command!");
}
