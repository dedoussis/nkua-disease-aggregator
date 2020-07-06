#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

using namespace std;

enum record_type
{
    ENTER,
    EXIT
};

class Record
{
public:
    int recordID, age;
    record_type type;
    string patientFirstName, patientLastName, disease;

    Record(int recordID, record_type type, string patientFirstName, string patientLastName, string disease, int age)
    {
        this->recordID = recordID;
        this->type = type;
        this->patientFirstName = patientFirstName;
        this->patientLastName = patientLastName;
        this->disease = disease;
        this->age = age;
    }

    string stringify()
    {
        return "Record(recordID=" + to_string(recordID) + ")";
    }
};

typedef map<string, vector<Record>> dated_records;

vector<Record> parseFileRecords(filesystem::path filePath)
{
    ifstream infile(filePath);

    int recordID, age;
    string type, patientFirstName, patientLastName, disease;

    vector<Record> records;

    while (infile >> recordID >> type >> patientFirstName >> patientLastName >> disease >> age)
    {
        Record record(recordID, type == "ENTER" ? ENTER : EXIT, patientFirstName, patientLastName, disease, age);
        records.push_back(record);
    }

    return records;
}

map<string, dated_records> parseRecords(string inputDir)
{
    map<string, dated_records> records;

    for (auto &p : filesystem::recursive_directory_iterator(inputDir))
    {
        if (filesystem::is_regular_file(p))
        {
            string country = p.path().parent_path().filename().string();
            string date = p.path().filename().string();
            cout << "Prossecing: " + country + " " + date << endl;
            records[country][date] = parseFileRecords(p.path());
        }
    }

    return records;
}

int main()
{
    map<string, dated_records> records = parseRecords("./input_dir");
    cout << records["China"]["31-01-2020"][0].stringify() << endl;
}
