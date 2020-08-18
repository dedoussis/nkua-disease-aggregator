#ifndef COMMON_H
#define COMMON_H

#include <functional>
#include <sstream>
#include <string>
#include <variant>

enum class Command
{
    SearchPatientRecord,
    DiseaseFrequency,
    Exit
};

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

struct DiseaseFrequencyRequest
{
    std::string virusName, startDate, endDate, country;
};

struct SearchPatientRecordRequest
{
    std::string recordID;
};

struct ExitRequest
{
};

struct PrintableResponse
{
    std::string data;
};

template <typename T>
using Deserializer = std::function<T(std::string)>;

using Request = std::variant<DiseaseFrequencyRequest, SearchPatientRecordRequest, ExitRequest>;
using Response = std::variant<PrintableResponse>;

std::string serialize(Request object);
std::string serialize(Response object);

Deserializer<Request> getRequestDeserializer(Command type);
Deserializer<Response> getResponseDeserializer(Command type);

#endif
