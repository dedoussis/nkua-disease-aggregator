#include "worker.h"
#include "fifo.h"
#include "internal.h"
#include <filesystem>
#include <fstream>
#include <signal.h>
#include <string>
#include <unistd.h>
#include <variant>
#include <vector>

std::vector<Record> parseFileRecords(std::filesystem::path filePath)
{
    std::ifstream infile(filePath);

    int age;
    std::string recordID, type, patientFirstName, patientLastName, disease;

    std::vector<Record> records;

    while (infile >> recordID >> type >> patientFirstName >> patientLastName >> disease >> age)
    {
        Record record = {recordID, patientFirstName, patientLastName, disease, age, type == "ENTER" ? RecordType::Enter : RecordType::Exit};
        records.push_back(record);
    }

    return records;
}

const Range RANGES[] = {{0, 20}, {21, 40}, {41, 60}, {61, std::numeric_limits<int>::max()}};

DiseaseStats generateSummaryStats(std::vector<Record> records)
{
    DiseaseStats diseaseStats;
    for (auto record : records)
    {
        for (auto range : RANGES)
        {
            auto [min, max] = range;
            if (min <= record.age && record.age <= max)
            {
                diseaseStats[record.disease][range]++;
                break;
            }
        }
    }
    return diseaseStats;
}

std::string generateReport(std::string date, std::string country, DiseaseStats diseaseStats)
{
    std::string report = date + NL + country + NL;
    for (auto [disease, stats] : diseaseStats)
    {
        report += disease + NL;
        for (auto range : RANGES)
        {
            auto [min, max] = range;
            int count = stats.contains(range) ? stats[range] : 0;
            report += "Age range " + std::to_string(min) + "-" + std::to_string(max) + " years: " + std::to_string(count) + " cases" + NL;
        }

        report += NL;
    }
    return report;
}

struct Handler
{
    Worker *worker;

    Internal::Response operator()(Internal::SummaryStatisticsRequest request)
    {
        Internal::RenderedResponse res;
        for (auto countryPath : request.filePaths)
        {
            for (auto &dateDirEntry : std::filesystem::directory_iterator(countryPath))
            {
                if (std::filesystem::is_regular_file(dateDirEntry))
                {
                    std::filesystem::path datePath = dateDirEntry.path();
                    std::string country = countryPath.filename().string();
                    std::string date = datePath.filename().string();
                    std::vector<Record> fileRecords = parseFileRecords(datePath);
                    DiseaseStats fileStats = generateSummaryStats(fileRecords);

                    worker->data.summaryStats[date][country] = fileStats;
                    worker->data.records[country][date] = fileRecords;
                    res.renderedString += generateReport(date, country, fileStats);
                }
            }
        }
        return res;
    }

    Internal::Response operator()(Internal::DiseaseFrequencyRequest request)
    {
        Internal::RenderedResponse res;
        res.renderedString = "Computing disease frequency for dieasease: " + request.virusName;
        return res;
    }
    Internal::Response operator()(Internal::SearchPatientRecordRequest request)
    {
        Internal::SearchPatientRecordResponse response;
        for (auto const &[country, datedRecords] : worker->data.records)
        {
            for (auto const &[date, records] : datedRecords)
            {
                for (auto record : records)
                {
                    if (record.recordID == request.recordID)
                    {
                        response.records.push_back(record);
                        return response;
                    }
                }
            }
        }
        return response;
    }
};

void Worker::stop()
{
    std::string outFileName = "log_file." + std::to_string(getPid());
    std::ofstream outFile(outFileName);
    for (const auto &kvPair : data.records)
        outFile << kvPair.first << std::endl;

    Internal::ExitResponse response = {.logFile = outFileName};
    m_queue.enqueue(Command::Exit, Internal::serialize(response));

    exit(0);
}

Worker *currentWorker;

void Worker::start()
{
    currentWorker = this;
    signal(SIGQUIT, [](int sig) { currentWorker->stop(); });

    for (;;)
    {
        auto [type, serializedInput] = m_queue.dequeue();

        Deserializer<Internal::Request> deserializer = Internal::getRequestDeserializer(type);
        Internal::Request request = deserializer(serializedInput);
        Handler handler = {.worker = this};
        Internal::Response response = visit(handler, request);

        m_queue.enqueue(type, Internal::serialize(response));
    }
}

Fifo Worker::getQueue() const
{
    return m_queue;
}

void Worker::setQueue(Fifo queue)
{
    m_queue = queue;
}

pid_t Worker::getPid() const
{
    return m_pid;
}

void Worker::setPid(pid_t pid)
{
    m_pid = pid;
}
