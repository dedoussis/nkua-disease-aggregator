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

template <typename T>
using Setter = std::function<void(T)>;

template <typename T>
using Getter = std::function<T()>;

struct Handler
{
    Getter<WorkerData> dataGetter;
    Setter<WorkerData> dataSetter;

    Internal::Response operator()(Internal::SummaryStatisticsRequest request)
    {
        WorkerData data;
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

                    data.summaryStats[date][country] = fileStats;
                    data.records[country][date] = fileRecords;
                    res.renderedString += generateReport(date, country, fileStats);
                }
            }
        }
        dataSetter(data);
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
        for (auto const &[country, datedRecords] : dataGetter().records)
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
    for (const auto &kvPair : m_data.records)
        outFile << kvPair.first << std::endl;

    Internal::ExitResponse response = {.logFile = outFileName};
    m_queue.enqueue(Command::Exit, Internal::serialize(response));

    exit(0);
}

void Worker::start()
{
    signal(SIGQUIT, [](int sig) { Worker::getInstance().stop(); });

    for (;;)
    {
        auto [type, serializedInput] = m_queue.dequeue();

        Deserializer<Internal::Request> deserializer = Internal::getRequestDeserializer(type);
        Internal::Request request = deserializer(serializedInput);
        Handler handler = {
            .dataGetter = [this]() { return this->getData(); },
            .dataSetter = [this](WorkerData data) { this->setData(data); }};
        Internal::Response response = visit(handler, request);

        m_queue.enqueue(type, Internal::serialize(response));
    }
}

Fifo WorkerSettings::getQueue() const
{
    return m_queue;
}

void WorkerSettings::setQueue(Fifo queue)
{
    m_queue = queue;
}

pid_t WorkerSettings::getPid() const
{
    return m_pid;
}

void WorkerSettings::setPid(pid_t pid)
{
    m_pid = pid;
}

void Worker::setSettings(WorkerSettings settings)
{
    setPid(settings.getPid());
    setQueue(settings.getQueue());
}

WorkerData Worker::getData() const
{
    return m_data;
}

void Worker::setData(WorkerData data)
{
    m_data = data;
}
