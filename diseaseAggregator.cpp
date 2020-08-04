#include "args.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <vector>

using namespace std;

enum class RecordType
{
    Enter,
    Exit
};

struct Record
{
    string recordID, patientFirstName, patientLastName, disease;
    int age;
    RecordType type;

    string stringify()
    {
        return recordID + " " + patientFirstName + " " + patientLastName + " " + disease + " " + to_string(age);
    }
};

using dated_records = map<string, vector<Record>>;

vector<Record> parseFileRecords(filesystem::path filePath)
{
    ifstream infile(filePath);

    int age;
    string recordID, type, patientFirstName, patientLastName, disease;

    vector<Record> records;

    while (infile >> recordID >> type >> patientFirstName >> patientLastName >> disease >> age)
    {
        Record record = {recordID, patientFirstName, patientLastName, disease, age, type == "ENTER" ? RecordType::Enter : RecordType::Exit};
        records.push_back(record);
    }

    return records;
}

using range = tuple<int, int>;
using stats = map<range, int>;
using disease_stats = map<string, stats>;

const range RANGES[] = {{0, 20}, {21, 40}, {41, 60}, {61, numeric_limits<int>::max()}};

disease_stats generateSummaryStats(vector<Record> records)
{
    disease_stats diseaseStats;
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

string generateReport(string date, string country, disease_stats diseaseStats)
{
    string report = date + NL + country + NL;
    for (auto [disease, stats] : diseaseStats)
    {
        report += disease + NL;
        for (auto range : RANGES)
        {
            auto [min, max] = range;
            int count = stats.contains(range) ? stats[range] : 0;
            report += "Age range " + to_string(min) + "-" + to_string(max) + " years: " + to_string(count) + " cases" + NL;
        }

        report += NL;
    }
    return report;
}

using records = map<string, dated_records>;
using summary_stats = map<string, map<string, disease_stats>>;

tuple<records, summary_stats> loadRecords(string inputDir)
{
    map<string, dated_records> loadedRecords;
    map<string, map<string, disease_stats>> summaryStats;
    for (auto &p : filesystem::recursive_directory_iterator(inputDir))
    {
        if (filesystem::is_regular_file(p))
        {
            string country = p.path().parent_path().filename().string();
            string date = p.path().filename().string();
            vector<Record> fileRecords = parseFileRecords(p.path());
            disease_stats fileStats = generateSummaryStats(fileRecords);
            cout << generateReport(date, country, fileStats);

            summaryStats[date][country] = fileStats;
            loadedRecords[country][date] = fileRecords;
        }
    }

    return make_tuple(loadedRecords, summaryStats);
}

struct Visitor
{
    records loadedRecords;
    summary_stats summaryStats;

    string operator()(DiseaseFrequencyOptions opts)
    {
        return "Computing disease frequency for dieasease: " + opts.virusName;
    }
    string operator()(SearchPatientRecordOptions opts)
    {
        for (auto const &[country, datedRecords] : loadedRecords)
        {
            for (auto const &[date, records] : datedRecords)
            {
                for (auto record : records)
                {
                    if (record.recordID == opts.recordID)
                    {
                        return record.stringify();
                    }
                }
            }
        }
        return "Patient not found.";
    }
    string operator()(ExitOptions opts)
    {
        return "Bye!";
    }
};

int main(int argc, char *argv[])
{
    InitialOptions options = parseInitialArgs(argc, argv);
    auto [loadedRecords, summaryStats] = loadRecords(options.inputDir);

    Command command = Command();
    while (command != Command::Exit)
    {
        cout << "Enter your command:" << endl;
        string inputString;
        getline(cin, inputString);
        try
        {
            auto [newCommand, opts] = parseInputString(inputString);
            command = newCommand;

            Visitor visitor = {.loadedRecords = loadedRecords, .summaryStats = summaryStats};
            string output = visit(visitor, opts);

            cout << output << endl;
        }
        catch (const std::runtime_error &error)
        {
            cerr << error.what() << endl;
        }
    }

    return 0;
}
