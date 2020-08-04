
#include <unistd.h>
#include <iostream>
#include <map>
#include "args.h"
#include <vector>
#include <sstream>

InitialOptions parseInitialArgs(int argc, char* argv[]) {
    const std::string helpMessage = "Usage: " + std::string(argv[0]) + " <option(s)>" + NL
        + "Options:" + NL
        + TAB + "-w NUMBER_OF_WORKERS" + TAB + "The number of workers to use" + NL
        + TAB + "-b BUFFER_SIZE" + TAB + TAB + "The size of the buffer" + NL
        + TAB + "-i INPUT_DIR" + TAB + TAB + "Directory containing all the input data" + NL
        + TAB + "-h" + TAB + TAB + TAB + "Show this help message";

    if (argc <= 1)
        throw helpMessage;

    InitialOptions opts;
    int c;

    while ((c = getopt(argc, argv, "w:b:i:h")) != -1)
    {
        switch (c)
        {
        case 'w':
            opts.numWorkers = std::stoi(optarg);
            break;
        case 'b':
            opts.bufferSize = std::stoi(optarg);
            break;
        case 'i':
            opts.inputDir = optarg;
            break;
        default:
            std::cout << helpMessage << std::endl;
            std::abort();
        }
    }
    return opts;
}

DiseaseFrequencyOptions parseDiseaseFrequencyArgs(std::vector<std::string> args)
{
    const std::string helpMessage = "Usage: " + std::string(args[0]) + " virusName startDate endDate [country]";
    if (args.size() < 4)
        throw std::runtime_error(helpMessage);

    DiseaseFrequencyOptions opts = {.virusName = args[1], .startDate = args[2], .endDate = args[3]};

    if (args.size() > 5)
    {
        opts.country = args[4];
    }

    return opts;
}

SearchPatientRecordOptions parseSearchPatientRecordArgs(std::vector<std::string> args)
{
    const std::string helpMessage = "Usage: " + std::string(args[0]) + " recordID";

    if (args.size() < 2)
        throw std::runtime_error(helpMessage);

    SearchPatientRecordOptions opts = {.recordID = args[1]};

    return opts;
}

std::map<std::string, Command> commandRegistry = {
    {"diseaseFrequency", Command::DiseaseFrequency},
    {"searchPatientRecord", Command::SearchPatientRecord},
    {"exit", Command::Exit},
};

std::tuple<Command, Options> parseInputString(std::string inputString)
{
    Command command;

    std::vector<std::string> args;
    std::stringstream inputStream(inputString);
    std::string arg;
    while (std::getline(inputStream, arg, ' '))
        args.push_back(arg);

    if (!args.empty() && commandRegistry.contains(args[0]))
    {
        command = commandRegistry[args[0]];
        switch (command)
        {
        case Command::DiseaseFrequency:
            return std::make_tuple(command, parseDiseaseFrequencyArgs(args));
        case Command::SearchPatientRecord:
            return std::make_tuple(command, parseSearchPatientRecordArgs(args));
        case Command::Exit:
            return std::make_tuple(command, ExitOptions());
        }
    }
    throw std::runtime_error("Invalid command!");
}