#include "external.h"
#include "common.h"
#include <map>
#include <variant>

namespace External
{
    struct ResponseSerializer
    {
        std::string operator()(RenderedResponse &response)
        {
            return response.renderedString;
        }

        std::string operator()(SearchPatientRecordResponse &response)
        {
            std::string serialized = join("Patients found:", response.records.size()) + NL;

            for (auto record : response.records)
                serialized += record.serialize() + NL;

            return serialized;
        }

        std::string operator()(ExitResponse &response)
        {
            std::string serliazed;

            for (auto [pid, logFile] : response.killedWorkers)
                serliazed += join("Worker", pid, "has been killed - Log file:", logFile) + NL;

            serliazed += "Bye! 👋";
            return serliazed;
        }

        std::string operator()(ListCountriesResponse &response)
        {
            std::string serialized;
            for (auto [country, pid] : response.countries)
                serialized += join(country, "- PID:", pid) + NL;

            return serialized;
        }
    };

    std::string serialize(Response object)
    {
        return visit(ResponseSerializer(), object);
    }

    DiseaseFrequencyRequest deseaseFrequencyRequestDeserialize(std::string payload)
    {
        std::stringstream payloadStream(payload);
        DiseaseFrequencyRequest req;

        payloadStream >> req.virusName >> req.startDate >> req.endDate >> req.country;

        return req;
    }

    SearchPatientRecordRequest searchPatientRecordRequestDeserialize(std::string payload)
    {
        std::stringstream payloadStream(payload);
        SearchPatientRecordRequest req;

        payloadStream >> req.recordID;

        return req;
    }

    SummaryStatisticsRequest summaryStatisticsRequestDeserialize(std::string payload)
    {
        SummaryStatisticsRequest request;

        for (auto token : split(payload))
            request.filePaths.push_back(std::filesystem::path(token));

        return request;
    }

    std::map<Command, Deserializer<Request>> requestDeserializerRegistry = {
        {Command::DiseaseFrequency, deseaseFrequencyRequestDeserialize},
        {Command::SearchPatientRecord, searchPatientRecordRequestDeserialize},
        {Command::SummaryStatistics, summaryStatisticsRequestDeserialize},
        {Command::ListCountries, [](std::string _) { return ListCountriesRequest(); }},
        {Command::Exit, [](std::string _) { return ExitRequest(); }}};

    Deserializer<Request> getRequestDeserializer(Command type)
    {
        return requestDeserializerRegistry[type];
    }
} // namespace External
