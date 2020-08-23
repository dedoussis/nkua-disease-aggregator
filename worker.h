#ifndef WORKER_H
#define WORKER_H

#include "common.h"
#include "fifo.h"
#include <filesystem>
#include <string>

struct WorkerData
{
    Records records;
    SummaryStats summaryStats;
};

struct Worker
{
private:
    Fifo m_queue;
    pid_t m_pid;

public:
    WorkerData data;
    Worker() = default;
    void start();
    void stop();
    Fifo getQueue() const;
    void setQueue(Fifo queue);
    pid_t getPid() const;
    void setPid(pid_t pid);
};

#endif