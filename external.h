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

    struct ExitRequest
    {
        std::string recordID;
    };

    struct RenderedResponse
    {
        std::string renderedString;
    };

    struct SearchPatientRecordResponse
    {
        std::vector<Record> records;
    };

    struct ExitResponse
    {
        std::vector<std::pair<pid_t, std::string>> killedWorkers;
    };

    using Request = std::variant<DiseaseFrequencyRequest, SearchPatientRecordRequest, SummaryStatisticsRequest, ExitRequest>;
    using Response = std::variant<RenderedResponse, SearchPatientRecordResponse, ExitResponse>;

    std::string serialize(Response object);

    Deserializer<Request> getRequestDeserializer(Command type);
} // namespace External

#endif
