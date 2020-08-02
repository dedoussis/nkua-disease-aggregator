#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <tuple>
#include <vector>

using namespace std;

enum record_type
{
    ENTER,
    EXIT
};

struct Record
{
    string recordID, patientFirstName, patientLastName, disease;
    int age;
    record_type type;

    string stringify()
    {
        return "Record(recordID=" + recordID + ")";
    }
};

typedef map<string, vector<Record>> dated_records;

vector<Record> parseFileRecords(filesystem::path filePath)
{
    ifstream infile(filePath);

    int age;
    string recordID, type, patientFirstName, patientLastName, disease;

    vector<Record> records;

    while (infile >> recordID >> type >> patientFirstName >> patientLastName >> disease >> age)
    {
        Record record = {recordID, patientFirstName, patientLastName, disease, age, type == "ENTER" ? ENTER : EXIT};
        records.push_back(record);
    }

    return records;
}

typedef tuple<int, int> range;
typedef map<range, int> stats;
typedef map<string, stats> disease_stats;

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
    const string NEWLINE = "\n";
    string report = date + NEWLINE + country + NEWLINE;
    for (auto [disease, stats] : diseaseStats)
    {
        report += disease + NEWLINE;
        for (auto range : RANGES)
        {
            auto [min, max] = range;
            int count = stats.contains(range) ? stats[range] : 0;
            report += "Age range " + to_string(min) + "-" + to_string(max) + " years: " + to_string(count) + " cases" + NEWLINE;
        }

        report += NEWLINE;
    }
    return report;
}

map<string, dated_records> parseRecords(string inputDir)
{
    map<string, dated_records> records;
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
            records[country][date] = fileRecords;
        }
    }

    return records;
}

int main()
{
    map<string, dated_records> records = parseRecords("./input_dir");
    cout << records["China"]["31-01-2020"][0].stringify() << endl;
}
