#include "args.h"
#include "common.h"
#include "external.h"
#include <iostream>
#include <map>
#include <unistd.h>
#include <vector>

InitialArgs ParseInitialArgs(int argc, char *argv[]) {
  const std::string help_message =
      "Usage: " + std::string(argv[0]) + " <option(s)>" + kNL +
      "Options:" + kNL + kTAB + "-w NUMBER_OF_WORKERS" + kTAB +
      "The number of workers to use" + kNL + kTAB + "-b BUFFER_SIZE" + kTAB +
      kTAB + "The size of the buffer" + kNL + kTAB + "-i INPUT_DIR" + kTAB +
      kTAB + "Directory containing all the input data" + kNL + kTAB + "-h" +
      kTAB + kTAB + kTAB + "Show this help message";

  if (argc <= 1)
    throw help_message;

  InitialArgs args;
  int c;

  while ((c = getopt(argc, argv, "w:b:i:h")) != -1) {
    switch (c) {
    case 'w':
      args.num_workers = std::stoi(optarg);
      break;
    case 'b':
      args.buffer_size = std::stoi(optarg);
      break;
    case 'i':
      args.input_dir = optarg;
      break;
    default:
      throw std::invalid_argument(help_message);
    }
  }
  return args;
}

std::map<std::string, Command> command_registry = {
    {"diseaseFrequency", Command::kDiseaseFrequency},
    {"searchPatientRecord", Command::kSearchPatientRecord},
    {"exit", Command::kExit},
    {"listCountries", Command::kListCountries}};

std::tuple<Command, External::Request> ParseCommand(std::string input_string) {
  std::string command_string = input_string.substr(0, input_string.find(" "));
  std::string rest = command_string == input_string
                         ? ""
                         : input_string.substr(command_string.size() + 1);

  if (input_string.empty() || !command_registry.contains(command_string))
    throw std::invalid_argument("Invalid command!");

  Command command = command_registry[command_string];
  Deserializer<External::Request> deserializer =
      External::GetRequestDeserializer(command);
  return std::make_tuple(command, deserializer(rest));
}
