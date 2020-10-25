#include "external.h"
#include "common.h"
#include <map>
#include <variant>

namespace External {
struct ResponseSerializer {
  std::string operator()(RenderedResponse &response) {
    return response.rendered_string;
  }

  std::string operator()(SearchPatientRecordResponse &response) {
    std::string serialized =
        Join("Patients found:", response.records.size()) + kNL;

    for (auto record : response.records)
      serialized += "😷 " + record.Serialize() + kNL;

    return serialized;
  }

  std::string operator()(ExitResponse &response) {
    std::string serliazed;

    for (auto [pid, log_file] : response.killed_workers)
      serliazed +=
          Join("Worker", pid, "has been killed - Log file:", log_file) + kNL;

    serliazed += "Bye! 👋";
    return serliazed;
  }

  std::string operator()(ListCountriesResponse &response) {
    std::string serialized;
    for (auto [country, pid] : response.countries)
      serialized += Join(country, "- PID:", pid) + kNL;

    return serialized;
  }
};

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

std::map<Command, Deserializer<Request>> request_deserializer_registry = {
    {Command::kDiseaseFrequency, DiseaseFrequencyRequestDeserialize},
    {Command::kSearchPatientRecord, SearchPatientRecordRequestDeserialize},
    {Command::kSummaryStatistics, SummaryStatisticsRequestDeserialize},
    {Command::kListCountries,
     [](std::string _) { return ListCountriesRequest(); }},
    {Command::kExit, [](std::string _) { return ExitRequest(); }}};

Deserializer<Request> GetRequestDeserializer(Command type) {
  return request_deserializer_registry[type];
}
} // namespace External
