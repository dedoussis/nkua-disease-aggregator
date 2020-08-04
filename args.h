#ifndef ARGS_H
#define ARGS_H

#include <iostream>
#include <map>
#include <variant>
#include <vector>

const std::string NL = "\n";
const std::string TAB = "\t";

struct InitialOptions
{
    std::string inputDir;
    int bufferSize, numWorkers;
};

struct DiseaseFrequencyOptions
{
    std::string virusName, startDate, endDate, country;
};

struct TopKAgeRangesOptions
{
    int k;
    std::string country, disease, startDate, endDate;
};

struct SearchPatientRecordOptions
{
    std::string recordID;
};

struct NumPatientAdmissionsOptions
{
    std::string virusName, startDate, endDate, country;
};

struct ExitOptions
{
};

enum class Command
{
    DiseaseFrequency,
    SearchPatientRecord,
    Exit,
};

InitialOptions parseInitialArgs(int argc, char *argv[]);
DiseaseFrequencyOptions parseDiseaseFrequencyArgs(std::vector<std::string> args);
SearchPatientRecordOptions parseSearchPatientRecordArgs(std::vector<std::string> args);

using Options = std::variant<DiseaseFrequencyOptions, SearchPatientRecordOptions, ExitOptions>;

std::tuple<Command, Options> parseInputString(std::string inputString);

using option_parser = Options (*)(std::vector<std::string>);

#endif
