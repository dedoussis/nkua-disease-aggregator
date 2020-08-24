#ifndef FIFO_H
#define FIFO_H

#include "common.h"
#include <string>
#include <utility>

struct Fifo
{
private:
    std::string m_path;
    int m_bufferSize;

public:
    Fifo(std::string path, int bufferSize);
    Fifo() = default;
    void make();
    void destroy();
    void enqueue(Command type, std::string payload);
    std::pair<Command, std::string> dequeue();
};

#endif