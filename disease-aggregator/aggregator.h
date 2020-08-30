#ifndef AGGREGATOR_H
#define AGGREGATOR_H

#include "common.h"
#include "external.h"
#include "worker.h"
#include <map>
#include <vector>

struct Aggregator
{
private:
    std::vector<WorkerSettings> workers;
    std::map<std::string, pid_t> countryMap;
    inline void addCountryWorker(std::string country, size_t workerIdx)
    {
        countryMap[country] = workerIdx;
    }
    inline WorkerSettings getCountryWorker(std::string country)
    {
        return workers[countryMap[country]];
    }

public:
    ~Aggregator();
    bool isInitialised();
    void addWorker(WorkerSettings worker);
    External::Response operator()(External::SummaryStatisticsRequest &request);
    External::Response operator()(External::DiseaseFrequencyRequest &request);
    External::Response operator()(External::SearchPatientRecordRequest &request);
    External::Response operator()(External::ListCountriesRequest &request);
    External::Response operator()(External::ExitRequest &request);
};

#endif
