#include "common.h"
#include <map>
#include <sstream>
#include <string>
#include <variant>

std::vector<std::string> Split(const std::string &s, char delimeter) {
  std::vector<std::string> tokens;
  std::stringstream ss(s);
  std::string token;

  while (std::getline(ss, token, delimeter))
    tokens.push_back(token);

  return tokens;
}

std::string Record::Serialize() {
  return Join(record_id, patient_first_name, patient_last_name, disease, age);
}

Record Record::Deserialize(std::string data) {
  std::stringstream data_stream(data);
  Record record;
  std::string age_string;
  data_stream >> record.record_id >> record.patient_first_name >>
      record.patient_last_name >> record.disease >> age_string;
  record.age = std::stoi(age_string);
  return record;
}
