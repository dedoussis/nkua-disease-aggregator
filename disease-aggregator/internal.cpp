#include "internal.h"
#include "common.h"
#include <map>
#include <variant>

namespace Internal {
struct RequestSerializer {
  std::string operator()(DiseaseFrequencyRequest &request) {
    return Join(request.virus_name, request.start_date, request.end_date,
                request.country);
  }

  std::string operator()(SearchPatientRecordRequest &request) {
    return request.record_id;
  }

  std::string operator()(SummaryStatisticsRequest &request) {
    std::string serialized;
    for (auto filePath : request.file_paths)
      serialized = Join(serialized, filePath.c_str());
    return serialized;
  }
};

struct ResponseSerializer {
  std::string operator()(RenderedResponse &response) {
    return response.rendered_string;
  }

  std::string operator()(SearchPatientRecordResponse &response) {
    std::string serialized;

    if (response.records.empty())
      return serialized;

    for (auto record : response.records)
      serialized += record.Serialize() + kNL;

    return serialized.substr(0, serialized.size() - 1);
  }

  std::string operator()(SummaryStatisticsResponse &response) {
    std::string serialized;

    for (auto country : response.countries)
      serialized = Join(serialized, country);

    return serialized + kVTAB + response.rendered_string;
  }

  std::string operator()(ExitResponse &response) { return response.log_file; }
};

std::string Serialize(Request object) {
  return visit(RequestSerializer(), object);
}

std::string Serialize(Response object) {
  return visit(ResponseSerializer(), object);
}

DiseaseFrequencyRequest
DiseaseFrequencyRequestDeserialize(std::string payload) {
  std::stringstream payload_stream(payload);
  DiseaseFrequencyRequest req;

  payload_stream >> req.virus_name >> req.start_date >> req.end_date >>
      req.country;

  return req;
}

SearchPatientRecordRequest
SearchPatientRecordRequestDeserialize(std::string payload) {
  std::stringstream payload_stream(payload);
  SearchPatientRecordRequest req;

  payload_stream >> req.record_id;

  return req;
}

SummaryStatisticsRequest
SummaryStatisticsRequestDeserialize(std::string payload) {
  SummaryStatisticsRequest request;

  for (auto token : Split(payload))
    request.file_paths.push_back(std::filesystem::path(token));

  return request;
}

RenderedResponse RenderedResponseDeserialize(std::string payload) {
  return RenderedResponse(payload);
}

SearchPatientRecordResponse
SearchPatientRecordResponseDeserialize(std::string payload) {

  SearchPatientRecordResponse response;
  for (auto serialized_record : Split(payload, kNL))
    response.records.push_back(Record::Deserialize(serialized_record));

  return response;
}

SummaryStatisticsResponse
SummaryStatisticsResponseDeserialize(std::string payload) {
  SummaryStatisticsResponse response;
  auto payload_vector = Split(payload, kVTAB);
  response.countries = Split(payload_vector[0]);
  response.rendered_string = payload_vector[1];
  return response;
}

ExitResponse ExitResponseDeserialize(std::string payload) {
  ExitResponse response = {.log_file = payload};
  return response;
}

std::map<Command, Deserializer<Request>> request_deserializer_registry = {
    {Command::kDiseaseFrequency, DiseaseFrequencyRequestDeserialize},
    {Command::kSearchPatientRecord, SearchPatientRecordRequestDeserialize},
    {Command::kSummaryStatistics, SummaryStatisticsRequestDeserialize}};

std::map<Command, Deserializer<Response>> response_deserializer_registry = {
    {Command::kDiseaseFrequency, RenderedResponseDeserialize},
    {Command::kSearchPatientRecord, SearchPatientRecordResponseDeserialize},
    {Command::kExit, ExitResponseDeserialize},
    {Command::kSummaryStatistics, SummaryStatisticsResponseDeserialize}};

Deserializer<Request> GetRequestDeserializer(Command type) {
  return request_deserializer_registry[type];
}

Deserializer<Response> GetResponseDeserializer(Command type) {
  return response_deserializer_registry[type];
}
} // namespace Internal
