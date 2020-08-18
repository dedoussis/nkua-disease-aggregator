#include "common.h"
#include <map>
#include <sstream>
#include <string>
#include <variant>

struct RequestSerializer
{
    std::string operator()(DiseaseFrequencyRequest request)
    {
        return join(request.virusName, request.startDate, request.endDate, request.country);
    }

    std::string operator()(SearchPatientRecordRequest request)
    {
        return request.recordID;
    }

    std::string operator()(ExitRequest request)
    {
        return std::string();
    }
};

struct ResponseSerializer
{
    std::string operator()(PrintableResponse response)
    {
        return response.data;
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

ExitRequest exitRequestDeserialize(std::string payload)
{
    return ExitRequest();
}

PrintableResponse printResponseDeserialize(std::string payload)
{
    return PrintableResponse();
}

std::map<Command, Deserializer<Request>> requestDeserializerRegistry = {
    {Command::DiseaseFrequency, deseaseFrequencyRequestDeserialize},
    {Command::SearchPatientRecord, searchPatientRecordRequestDeserialize},
    {Command::Exit, exitRequestDeserialize}};

std::map<Command, Deserializer<Response>> responseDeserializerRegistry = {
    {Command::DiseaseFrequency, printResponseDeserialize},
    {Command::SearchPatientRecord, printResponseDeserialize},
    {Command::Exit, printResponseDeserialize}};

Deserializer<Request> getRequestDeserializer(Command type)
{
    return requestDeserializerRegistry[type];
}

Deserializer<Response> getResponseDeserializer(Command type)
{
    return responseDeserializerRegistry[type];
}