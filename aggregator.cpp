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

template <typename T>
inline std::vector<T> extendVector(std::vector<T> v1, std::vector<T> v2)
{
    std::vector<T> extended(v1);
    extended.reserve(extended.size() + v2.size());
    extended.insert(extended.end(), v2.begin(), v2.end());
    return extended;
}

void Aggregator::addWorker(Worker worker)
{
    workers.push_back(worker);
}

template <typename Item>
std::vector<std::vector<Item>> group(std::vector<Item> v, size_t groups)
{
    std::vector<std::vector<Item>> grouppedVector(groups);
    for (size_t i = 0; i < v.size(); i++)
        grouppedVector[i % groups].push_back(v[i]);

    return grouppedVector;
}

Aggregator::~Aggregator()
{
    for (auto worker : workers)
        worker.getQueue().destroy();
}

External::Response Aggregator::operator()(External::SummaryStatisticsRequest request)
{
    External::RenderedResponse response;
    std::vector<std::vector<fs::path>> grouppedVector = group<fs::path>(request.filePaths, workers.size());

    for (size_t i = 0; i < grouppedVector.size(); i++)
    {
        Worker worker = workers[i];
        Internal::SummaryStatisticsRequest workerRequest = {.filePaths = grouppedVector[i]};
        worker.getQueue().enqueue(Command::SummaryStatistics, Internal::serialize(workerRequest));
        auto [type, output] = worker.getQueue().dequeue();
        response.renderedString += output;
    }
    return response;
}

External::Response Aggregator::operator()(External::DiseaseFrequencyRequest request)
{
    External::RenderedResponse response = {.renderedString = "Hello from aggregator!"};
    return response;
}

External::Response Aggregator::operator()(External::SearchPatientRecordRequest request)
{
    External::SearchPatientRecordResponse response;

    for (auto worker : workers)
    {
        Internal::SearchPatientRecordRequest workerRequest = {.recordID = request.recordID};
        worker.getQueue().enqueue(Command::SearchPatientRecord, Internal::serialize(workerRequest));
        auto [type, output] = worker.getQueue().dequeue();
        Internal::SearchPatientRecordResponse workerResponse = Internal::searchPatientRecordResponseDeserialize(output);
        response.records = extendVector<Record>(response.records, workerResponse.records);
    }

    return response;
}

External::Response Aggregator::operator()(External::ExitRequest request)
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
