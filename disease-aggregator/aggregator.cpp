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

void Aggregator::add_worker(WorkerSettings worker)
{
    workers_.push_back(worker);
}

Aggregator::~Aggregator()
{
    for (auto worker : workers_)
        worker.get_queue().Destroy();
}

bool Aggregator::IsInitialised()
{
    return !country_map_.empty();
}

External::Response Aggregator::operator()(External::SummaryStatisticsRequest &request)
{
    External::RenderedResponse response;
    std::vector<std::vector<fs::path>> groupped_vector = Group<fs::path>(request.file_paths, workers_.size());

    for (size_t i = 0; i < groupped_vector.size(); i++)
    {
        WorkerSettings worker = workers_[i];
        Internal::SummaryStatisticsRequest worker_request = {.file_paths = groupped_vector[i]};
        worker.get_queue().Enqueue(Command::kSummaryStatistics, Internal::Serialize(worker_request));
    }

    for (size_t i = 0; i < workers_.size(); i++)
    {
        auto worker = workers_[i];
        auto [type, output] = worker.get_queue().Dequeue();
        Internal::SummaryStatisticsResponse worker_response = Internal::SummaryStatisticsResponseDeserialize(output);
        for (auto country : worker_response.countries)
            add_country_worker(country, i);
        response.rendered_string += worker_response.rendered_string;
    }

    return response;
}

External::Response Aggregator::operator()(External::DiseaseFrequencyRequest &request)
{
    External::RenderedResponse response = {.rendered_string = "TODO - Hello from aggregator!"};
    return response;
}

External::Response Aggregator::operator()(External::SearchPatientRecordRequest &request)
{
    External::SearchPatientRecordResponse response;

    for (auto worker : workers_)
    {
        Internal::SearchPatientRecordRequest worker_request = {.record_id = request.record_id};
        worker.get_queue().Enqueue(Command::kSearchPatientRecord, Internal::Serialize(worker_request));
    }

    for (auto worker : workers_)
    {
        auto [type, output] = worker.get_queue().Dequeue();
        Internal::SearchPatientRecordResponse worker_response = Internal::SearchPatientRecordResponseDeserialize(output);
        response.records = ExtendVector<Record>(response.records, worker_response.records);
    }

    return response;
}

External::Response Aggregator::operator()(External::ListCountriesRequest &request)
{
    External::ListCountriesResponse response;

    for (auto const &[country, worker_idx] : country_map_)
        response.countries.push_back(std::make_pair(country, workers_[worker_idx].get_pid()));

    return response;
}

External::Response Aggregator::operator()(External::ExitRequest &request)
{
    External::ExitResponse response;
    for (auto worker : workers_)
        kill(worker.get_pid(), SIGQUIT);

    for (auto worker : workers_)
    {
        auto response_pair = worker.get_queue().Dequeue();
        Internal::ExitResponse worker_response = Internal::ExitResponseDeserialize(response_pair.second);
        response.killed_workers.push_back(std::make_pair(worker.get_pid(), worker_response.log_file));
    }

    return response;
}
