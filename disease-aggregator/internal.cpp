#include "internal.h"
#include "common.h"
#include <map>
#include <variant>

namespace Internal
{
    struct RequestSerializer
    {
        std::string operator()(DiseaseFrequencyRequest &request)
        {
            return join(request.virusName, request.startDate, request.endDate, request.country);
        }

        std::string operator()(SearchPatientRecordRequest &request)
        {
            return request.recordID;
        }

        std::string operator()(SummaryStatisticsRequest &request)
        {
            std::string serialized;
            for (auto filePath : request.filePaths)
                serialized = join(serialized, filePath.c_str());
            return serialized;
        }
    };

    struct ResponseSerializer
    {
        std::string operator()(RenderedResponse &response)
        {
            return response.renderedString;
        }

        std::string operator()(SearchPatientRecordResponse &response)
        {
            std::string serialized;

            if (response.records.empty())
                return serialized;

            for (auto record : response.records)
                serialized += record.serialize() + NL;

            return serialized.substr(0, serialized.size() - 1);
        }

        std::string operator()(SummaryStatisticsResponse &response)
        {
            std::string serialized;

            for (auto country : response.countries)
                serialized = join(serialized, country);

            return serialized + VTAB + response.renderedString;
        }

        std::string operator()(ExitResponse &response)
        {
            return response.logFile;
        }
    };

    std::string serialize(Request object)
    {
        return visit(RequestSerializer(), object);
    }

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

    RenderedResponse renderedResponseDeserialize(std::string payload)
    {
        return RenderedResponse(payload);
    }

    SearchPatientRecordResponse searchPatientRecordResponseDeserialize(std::string payload)
    {

        SearchPatientRecordResponse response;
        for (auto serializedRecord : split(payload, NL))
            response.records.push_back(Record::deserialize(serializedRecord));

        return response;
    }

    SummaryStatisticsResponse summaryStatisticsResponseDeserialize(std::string payload)
    {
        SummaryStatisticsResponse response;
        auto payloadVector = split(payload, VTAB);
        response.countries = split(payloadVector[0]);
        response.renderedString = payloadVector[1];
        return response;
    }

    ExitResponse exitResponseDeserialize(std::string payload)
    {
        ExitResponse response = {.logFile = payload};
        return response;
    }

    std::map<Command, Deserializer<Request>> requestDeserializerRegistry = {
        {Command::DiseaseFrequency, deseaseFrequencyRequestDeserialize},
        {Command::SearchPatientRecord, searchPatientRecordRequestDeserialize},
        {Command::SummaryStatistics, summaryStatisticsRequestDeserialize}};

    std::map<Command, Deserializer<Response>> responseDeserializerRegistry = {
        {Command::DiseaseFrequency, renderedResponseDeserialize},
        {Command::SearchPatientRecord, searchPatientRecordResponseDeserialize},
        {Command::Exit, exitResponseDeserialize},
        {Command::SummaryStatistics, summaryStatisticsResponseDeserialize}};

    Deserializer<Request> getRequestDeserializer(Command type)
    {
        return requestDeserializerRegistry[type];
    }

    Deserializer<Response> getResponseDeserializer(Command type)
    {
        return responseDeserializerRegistry[type];
    }
} // namespace Internal
