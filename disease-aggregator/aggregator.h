#ifndef DISEASE_AGGREGATOR_AGGREGATOR_H
#define DISEASE_AGGREGATOR_AGGREGATOR_H

#include "common.h"
#include "external.h"
#include "worker.h"
#include <map>
#include <vector>

struct Aggregator {
private:
  std::vector<WorkerSettings> workers_;
  std::map<std::string, pid_t> country_map_;
  inline void add_country_worker(std::string country, size_t worker_idx) {
    country_map_[country] = worker_idx;
  }
  inline WorkerSettings get_country_worker(std::string country) {
    return workers_[country_map_[country]];
  }

public:
  ~Aggregator();
  bool IsInitialised();
  void add_worker(WorkerSettings worker);
  External::Response operator()(External::SummaryStatisticsRequest &request);
  External::Response operator()(External::DiseaseFrequencyRequest &request);
  External::Response operator()(External::SearchPatientRecordRequest &request);
  External::Response operator()(External::ListCountriesRequest &request);
  External::Response operator()(External::ExitRequest &request);
};

#endif
