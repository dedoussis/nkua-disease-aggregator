#include "aggregator.h"
#include "common.h"
#include "external.h"
#include "internal.h"
#include "worker.h"
#include <filesystem>
#include <map>
#include <signal.h>
#include <variant>
#include <vector>

namespace fs = std::filesystem;

void Aggregator::addWorker(WorkerSettings worker)
{
    workers.push_back(worker);
}

Aggregator::~Aggregator()
{
    for (auto worker : workers)
        worker.getQueue().destroy();
}

bool Aggregator::isInitialised()
{
    return !countryMap.empty();
}

External::Response Aggregator::operator()(External::SummaryStatisticsRequest &request)
{
    External::RenderedResponse response;
    std::vector<std::vector<fs::path>> grouppedVector = group<fs::path>(request.filePaths, workers.size());

    for (size_t i = 0; i < grouppedVector.size(); i++)
    {
        WorkerSettings worker = workers[i];
        Internal::SummaryStatisticsRequest workerRequest = {.filePaths = grouppedVector[i]};
        worker.getQueue().enqueue(Command::SummaryStatistics, Internal::serialize(workerRequest));
    }

    for (size_t i = 0; i < workers.size(); i++)
    {
        auto worker = workers[i];
        auto [type, output] = worker.getQueue().dequeue();
        Internal::SummaryStatisticsResponse workerResponse = Internal::summaryStatisticsResponseDeserialize(output);
        for (auto country : workerResponse.countries)
            addCountryWorker(country, i);
        response.renderedString += workerResponse.renderedString;
    }

    return response;
}

External::Response Aggregator::operator()(External::DiseaseFrequencyRequest &request)
{
    External::RenderedResponse response = {.renderedString = " TODO - Hello from aggregator!"};
    return response;
}

External::Response Aggregator::operator()(External::SearchPatientRecordRequest &request)
{
    External::SearchPatientRecordResponse response;

    for (auto worker : workers)
    {
        Internal::SearchPatientRecordRequest workerRequest = {.recordID = request.recordID};
        worker.getQueue().enqueue(Command::SearchPatientRecord, Internal::serialize(workerRequest));
    }

    for (auto worker : workers)
    {
        auto [type, output] = worker.getQueue().dequeue();
        Internal::SearchPatientRecordResponse workerResponse = Internal::searchPatientRecordResponseDeserialize(output);
        response.records = extendVector<Record>(response.records, workerResponse.records);
    }

    return response;
}

External::Response Aggregator::operator()(External::ExitRequest &request)
{
    External::ExitResponse response;
    for (auto worker : workers)
        kill(worker.getPid(), SIGQUIT);

    for (auto worker : workers)
    {
        auto responsePair = worker.getQueue().dequeue();
        Internal::ExitResponse workerResponse = Internal::exitResponseDeserialize(responsePair.second);
        response.killedWorkers.push_back(std::make_pair(worker.getPid(), workerResponse.logFile));
    }

    return response;
}
