#ifndef INTERNAL_H
#define INTERNAL_H

#include "common.h"
#include <variant>

namespace Internal
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

    struct RenderedResponse
    {
        std::string renderedString;
    };

    struct SearchPatientRecordResponse
    {
        std::vector<Record> records;
    };

    struct SummaryStatisticsResponse : RenderedResponse
    {
        std::vector<std::string> countries;
    };

    struct ExitResponse
    {
        std::string logFile;
    };

    using Request = std::variant<DiseaseFrequencyRequest, SearchPatientRecordRequest, SummaryStatisticsRequest>;
    using Response = std::variant<RenderedResponse, SearchPatientRecordResponse, SummaryStatisticsResponse, ExitResponse>;

    std::string serialize(Request object);
    std::string serialize(Response object);

    Deserializer<Request> getRequestDeserializer(Command type);
    Deserializer<Response> getResponseDeserializer(Command type);

    SearchPatientRecordResponse searchPatientRecordResponseDeserialize(std::string payload);
    SummaryStatisticsResponse summaryStatisticsResponseDeserialize(std::string payload);
    ExitResponse exitResponseDeserialize(std::string payload);
} // namespace Internal

#endif
