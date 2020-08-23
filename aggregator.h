#ifndef AGGREGATOR_H
#define AGGREGATOR_H

#include "common.h"
#include "external.h"
#include "worker.h"
#include <vector>

struct Aggregator
{
private:
    std::vector<Worker> workers;

public:
    ~Aggregator();
    void addWorker(Worker worker);
    External::Response operator()(External::SummaryStatisticsRequest request);
    External::Response operator()(External::DiseaseFrequencyRequest request);
    External::Response operator()(External::SearchPatientRecordRequest request);
    External::Response operator()(External::ExitRequest request);
};

#endif
