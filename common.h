#ifndef COMMON_H
#define COMMON_H

#include <string>
#include <functional>
#include <variant>

enum class Command {
    SearchPatientRecord,
    DiseaseFrequency,
    Exit
};

template <typename... Args>
std::string join(Args const &... args);


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

template<typename T>
using Deserializer = std::function<T(std::string)>;

using Request = std::variant<DiseaseFrequencyRequest, SearchPatientRecordRequest, ExitRequest>;
using Response = std::variant<PrintableResponse>;

std::string serialize(Request object);
std::string serialize(Response object);

Deserializer<Request> getRequestDeserializer(Command type);
Deserializer<Response> getResponseDeserializer(Command type);

#endif
