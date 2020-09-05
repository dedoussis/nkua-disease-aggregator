#include "worker.h"
#include "fifo.h"
#include "internal.h"
#include <filesystem>
#include <fstream>
#include <functional>
#include <signal.h>
#include <string>
#include <unistd.h>
#include <variant>
#include <vector>

namespace fs = std::filesystem;

std::vector<Record> ParseFileRecords(fs::path file_path) {
  std::ifstream infile(file_path);

  int age;
  std::string record_id, type, patient_first_name, patient_last_name, disease;

  std::vector<Record> records;

  while (infile >> record_id >> type >> patient_first_name >>
         patient_last_name >> disease >> age) {
    Record record = {record_id,
                     patient_first_name,
                     patient_last_name,
                     disease,
                     age,
                     type == "ENTER" ? RecordType::kEnter : RecordType::kExit};
    records.push_back(record);
  }

  return records;
}

const Range kRanges[] = {
    {0, 20}, {21, 40}, {41, 60}, {61, std::numeric_limits<int>::max()}};

DiseaseStats GenerateSummaryStats(std::vector<Record> records) {
  DiseaseStats disease_stats;
  for (auto record : records) {
    for (auto range : kRanges) {
      auto [min, max] = range;
      if (min <= record.age && record.age <= max) {
        disease_stats[record.disease][range]++;
        break;
      }
    }
  }
  return disease_stats;
}

std::string GenerateReport(std::string date, std::string country,
                           DiseaseStats disease_stats) {
  std::string report = date + kNL + country + kNL;
  for (auto [disease, stats] : disease_stats) {
    report += disease + kNL;
    for (auto range : kRanges) {
      auto [min, max] = range;
      int count = stats.contains(range) ? stats[range] : 0;
      report +=
          Join("Age range", min, "-", max, "years:", count, "cases") + kNL;
    }

    report += kNL;
  }
  return report;
}

template <typename T> using Setter = std::function<void(T)>;

template <typename T> using Getter = std::function<T()>;

struct Handler {
  Getter<WorkerData> data_getter;
  Setter<WorkerData> data_setter;

  Internal::Response operator()(Internal::SummaryStatisticsRequest &request) {
    WorkerData data;
    Internal::SummaryStatisticsResponse res;
    for (auto countryPath : request.file_paths) {
      std::string country = countryPath.filename().string();
      res.countries.push_back(country);
      for (auto &dateDirEntry : fs::directory_iterator(countryPath)) {
        if (fs::is_regular_file(dateDirEntry)) {
          fs::path datePath = dateDirEntry.path();
          std::string date = datePath.filename().string();
          std::vector<Record> file_records = ParseFileRecords(datePath);
          DiseaseStats file_stats = GenerateSummaryStats(file_records);

          data.summary_stats[date][country] = file_stats;
          data.records[country][date] = file_records;
          res.rendered_string += GenerateReport(date, country, file_stats);
        }
      }
    }
    data_setter(data);
    return res;
  }

  Internal::Response operator()(Internal::DiseaseFrequencyRequest &request) {
    Internal::RenderedResponse res;
    res.rendered_string =
        "Computing disease frequency for dieasease: " + request.virus_name;
    return res;
  }
  Internal::Response operator()(Internal::SearchPatientRecordRequest &request) {
    Internal::SearchPatientRecordResponse response;
    for (auto const &[country, dated_records] : data_getter().records) {
      for (auto const &[date, records] : dated_records) {
        for (auto record : records) {
          if (record.record_id == request.record_id) {
            response.records.push_back(record);
            return response;
          }
        }
      }
    }
    return response;
  }
};

void Worker::Stop() {
  std::string out_file_name = "log_file." + std::to_string(get_pid());
  std::ofstream out_file(out_file_name);
  for (const auto &kv_pair : data_.records)
    out_file << kv_pair.first << std::endl;

  Internal::ExitResponse response = {.log_file = out_file_name};
  queue_.Enqueue(Command::kExit, Internal::Serialize(response));

  exit(0);
}

void Worker::Start() {
  signal(SIGQUIT, [](int sig) { Worker::get_instance().Stop(); });

  for (;;) {
    auto [type, serializedInput] = queue_.Dequeue();

    Deserializer<Internal::Request> deserializer =
        Internal::GetRequestDeserializer(type);
    Internal::Request request = deserializer(serializedInput);
    Handler handler = {
        .data_getter = [this]() { return this->get_data(); },
        .data_setter = [this](WorkerData data) { this->set_data(data); }};
    Internal::Response response = visit(handler, request);

    queue_.Enqueue(type, Internal::Serialize(response));
  }
}

Fifo WorkerSettings::get_queue() const { return queue_; }

void WorkerSettings::set_queue(Fifo queue) { queue_ = queue; }

pid_t WorkerSettings::get_pid() const { return pid_; }

void WorkerSettings::set_pid(pid_t pid) { pid_ = pid; }

void Worker::set_settings(WorkerSettings settings) {
  set_pid(settings.get_pid());
  set_queue(settings.get_queue());
}

WorkerData Worker::get_data() const { return data_; }

void Worker::set_data(WorkerData data) { data_ = data; }
