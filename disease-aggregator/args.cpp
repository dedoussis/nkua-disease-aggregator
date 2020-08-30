#include "args.h"
#include "common.h"
#include "external.h"
#include <iostream>
#include <map>
#include <unistd.h>
#include <vector>

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
    {"listCountries", Command::ListCountries}};

std::tuple<Command, External::Request> parseCommand(std::string inputString)
{
    std::string commandString = inputString.substr(0, inputString.find(" "));
    std::string rest = commandString == inputString ? "" : inputString.substr(commandString.size() + 1);

    if (inputString.empty() || !commandRegistry.contains(commandString))
        throw std::runtime_error("Invalid command!");

    Command command = commandRegistry[commandString];
    Deserializer<External::Request> deserializer = External::getRequestDeserializer(command);
    return std::make_tuple(command, deserializer(rest));
}
