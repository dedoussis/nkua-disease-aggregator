#include "common.h"
#include <map>
#include <sstream>
#include <string>
#include <variant>

std::vector<std::string> split(const std::string &s, char delimeter)
{
    std::vector<std::string> tokens;
    std::stringstream ss(s);
    std::string token;

    while (std::getline(ss, token, delimeter))
        tokens.push_back(token);

    return tokens;
}

std::string Record::serialize()
{
    return join(recordID, patientFirstName, patientLastName, disease, age);
}

Record Record::deserialize(std::string data)
{
    std::stringstream dataStream(data);
    Record record;
    std::string ageString;
    dataStream >> record.recordID >> record.patientFirstName >> record.patientLastName >> record.disease >> ageString;
    record.age = std::stoi(ageString);
    return record;
}
