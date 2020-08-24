#ifndef AGGREGATOR_H
#define AGGREGATOR_H

#include "common.h"
#include "external.h"
#include "worker.h"
#include <vector>

struct Aggregator
{
private:
    std::vector<WorkerSettings> workers;

public:
    ~Aggregator();
    void addWorker(WorkerSettings worker);
    External::Response operator()(External::SummaryStatisticsRequest request);
    External::Response operator()(External::DiseaseFrequencyRequest request);
    External::Response operator()(External::SearchPatientRecordRequest request);
    External::Response operator()(External::ExitRequest request);
};

#endif
