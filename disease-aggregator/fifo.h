#ifndef DISEASE_AGGREGATOR_FIFO_H
#define DISEASE_AGGREGATOR_FIFO_H

#include "common.h"
#include <string>
#include <utility>

struct Fifo {
private:
  std::string path_;
  int buffer_size_;

public:
  Fifo(std::string path, int buffer_size);
  Fifo() = default;
  void Make();
  void Destroy();
  void Enqueue(Command type, std::string payload);
  std::pair<Command, std::string> Dequeue();
};

#endif