#ifndef DISEASE_AGGREGATOR_EXTERNAL_H
#define DISEASE_AGGREGATOR_EXTERNAL_H

#include "common.h"
#include <variant>

namespace External {
struct SummaryStatisticsRequest {
  std::vector<std::filesystem::path> file_paths;
};

struct DiseaseFrequencyRequest {
  std::string virus_name, start_date, end_date, country;
};

struct SearchPatientRecordRequest {
  std::string record_id;
};

struct ListCountriesRequest {};

struct ExitRequest {};

struct RenderedResponse {
  std::string rendered_string;
};

struct SearchPatientRecordResponse {
  std::vector<Record> records;
};

struct ListCountriesResponse {
  std::vector<std::pair<std::string, pid_t>> countries;
};

struct ExitResponse {
  std::vector<std::pair<pid_t, std::string>> killed_workers;
};

using Request =
    std::variant<DiseaseFrequencyRequest, SearchPatientRecordRequest,
                 SummaryStatisticsRequest, ListCountriesRequest, ExitRequest>;
using Response = std::variant<RenderedResponse, SearchPatientRecordResponse,
                              ListCountriesResponse, ExitResponse>;

std::string Serialize(Response object);

Deserializer<Request> GetRequestDeserializer(Command type);
} // namespace External

#endif
