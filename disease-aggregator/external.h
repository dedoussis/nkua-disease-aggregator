#ifndef EXTERNAL_H
#define EXTERNAL_H

#include "common.h"
#include <variant>

namespace External
{
    struct SummaryStatisticsRequest
    {
        std::vector<std::filesystem::path> filePaths;
    };

    struct DiseaseFrequencyRequest
    {
        std::string virusName, startDate, endDate, country;
    };

    struct SearchPatientRecordRequest
    {
        std::string recordID;
    };

    struct ListCountriesRequest
    {
    };

    struct ExitRequest
    {
    };

    struct RenderedResponse
    {
        std::string renderedString;
    };

    struct SearchPatientRecordResponse
    {
        std::vector<Record> records;
    };

    struct ListCountriesResponse
    {
        std::vector<std::pair<std::string, pid_t>> countries;
    };

    struct ExitResponse
    {
        std::vector<std::pair<pid_t, std::string>> killedWorkers;
    };

    using Request = std::variant<DiseaseFrequencyRequest, SearchPatientRecordRequest, SummaryStatisticsRequest, ListCountriesRequest, ExitRequest>;
    using Response = std::variant<RenderedResponse, SearchPatientRecordResponse, ListCountriesResponse, ExitResponse>;

    std::string serialize(Response object);

    Deserializer<Request> getRequestDeserializer(Command type);
} // namespace External

#endif
