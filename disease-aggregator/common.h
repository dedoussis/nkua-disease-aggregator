#ifndef DISEASE_AGGREGATOR_COMMON_H
#define DISEASE_AGGREGATOR_COMMON_H

#include <filesystem>
#include <functional>
#include <map>
#include <sstream>
#include <string>
#include <variant>
#include <vector>

enum class RecordType { kEnter, kExit };

struct Record {
  std::string record_id, patient_first_name, patient_last_name, disease;
  int age;
  RecordType type;

  std::string Serialize();
  static Record Deserialize(std::string data);
};

enum class Command {
  kSummaryStatistics,
  kSearchPatientRecord,
  kDiseaseFrequency,
  kListCountries,
  kExit
};

std::vector<std::string> Split(const std::string &s, char delimeter = ' ');

template <typename T> inline std::string Join(const T &t) {
  std::stringstream ss;
  ss << t;
  return ss.str();
}

template <typename T, typename... Args>
inline std::string Join(const T &first, Args... args) {
  std::string first_joined = Join(first);
  std::string args_joined = Join(args...);
  return first_joined.empty() ? args_joined : first_joined + " " + args_joined;
}

template <typename T>
inline std::vector<T> ExtendVector(std::vector<T> v1, std::vector<T> v2) {
  std::vector<T> extended(v1);
  extended.reserve(extended.size() + v2.size());
  extended.insert(extended.end(), v2.begin(), v2.end());
  return extended;
}

template <typename Item>
std::vector<std::vector<Item>> Group(std::vector<Item> v, size_t groups) {
  std::vector<std::vector<Item>> groupped_vector(groups);
  for (size_t i = 0; i < v.size(); i++)
    groupped_vector[i % groups].push_back(v[i]);

  return groupped_vector;
}

template <typename T> using Deserializer = std::function<T(std::string)>;

using Range = std::tuple<int, int>;
using Stats = std::map<Range, int>;
using DiseaseStats = std::map<std::string, Stats>;
using SummaryStats = std::map<std::string, std::map<std::string, DiseaseStats>>;

using DatedRecords = std::map<std::string, std::vector<Record>>;
using Records = std::map<std::string, DatedRecords>;

const char kNL = '\n';
const char kTAB = '\t';
const char kVTAB = '\v';

#endif
