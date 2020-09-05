#ifndef INTERNAL_H
#define INTERNAL_H

#include "common.h"
#include <variant>

namespace Internal {
struct SummaryStatisticsRequest {
  std::vector<std::filesystem::path> file_paths;
};

struct DiseaseFrequencyRequest {
  std::string virus_name, start_date, end_date, country;
};

struct SearchPatientRecordRequest {
  std::string record_id;
};

struct RenderedResponse {
  std::string rendered_string;
};

struct SearchPatientRecordResponse {
  std::vector<Record> records;
};

struct SummaryStatisticsResponse : RenderedResponse {
  std::vector<std::string> countries;
};

struct ExitResponse {
  std::string log_file;
};

using Request =
    std::variant<DiseaseFrequencyRequest, SearchPatientRecordRequest,
                 SummaryStatisticsRequest>;
using Response = std::variant<RenderedResponse, SearchPatientRecordResponse,
                              SummaryStatisticsResponse, ExitResponse>;

std::string Serialize(Request object);
std::string Serialize(Response object);

Deserializer<Request> GetRequestDeserializer(Command type);
Deserializer<Response> GetResponseDeserializer(Command type);

SearchPatientRecordResponse
SearchPatientRecordResponseDeserialize(std::string payload);
SummaryStatisticsResponse
SummaryStatisticsResponseDeserialize(std::string payload);
ExitResponse ExitResponseDeserialize(std::string payload);
} // namespace Internal

#endif
