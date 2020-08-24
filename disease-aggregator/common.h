#ifndef COMMON_H
#define COMMON_H

#include <filesystem>
#include <functional>
#include <map>
#include <sstream>
#include <string>
#include <variant>
#include <vector>

enum class RecordType
{
    Enter,
    Exit
};

struct Record
{
    std::string recordID, patientFirstName, patientLastName, disease;
    int age;
    RecordType type;

    std::string serialize();
    static Record deserialize(std::string data);
};

enum class Command
{
    SummaryStatistics,
    SearchPatientRecord,
    DiseaseFrequency,
    Exit
};

std::vector<std::string> split(const std::string &s, char delimeter = ' ');

template <typename T>
inline std::string join(const T &t)
{
    std::stringstream ss;
    ss << t;
    return ss.str();
}

template <typename T, typename... Args>
inline std::string join(const T &first, Args... args)
{
    std::string firstJoined = join(first);
    std::string argsJoined = join(args...);
    return firstJoined.empty() ? argsJoined : firstJoined + " " + argsJoined;
}

template <typename T>
using Deserializer = std::function<T(std::string)>;


using Range = std::tuple<int, int>;
using Stats = std::map<Range, int>;
using DiseaseStats = std::map<std::string, Stats>;
using SummaryStats = std::map<std::string, std::map<std::string, DiseaseStats>>;

using DatedRecords = std::map<std::string, std::vector<Record>>;
using Records = std::map<std::string, DatedRecords>;

const char NL = '\n';
const char TAB = '\t';

#endif