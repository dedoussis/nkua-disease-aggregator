#ifndef WORKER_H
#define WORKER_H

#include "common.h"
#include "fifo.h"
#include <filesystem>
#include <string>

struct WorkerSettings
{
protected:
    Fifo m_queue;
    pid_t m_pid;

public:
    Fifo getQueue() const;
    void setQueue(Fifo queue);
    pid_t getPid() const;
    void setPid(pid_t pid);
};
struct WorkerData
{
    Records records;
    SummaryStats summaryStats;
};

struct Worker : WorkerSettings
{
private:
    WorkerData m_data;
    Worker(){};

public:
    static Worker &getInstance()
    {
        static Worker instance;
        return instance;
    }
    Worker(Worker const &) = delete;
    void operator=(Worker const &) = delete;
    void start();
    void stop();
    WorkerData getData() const;
    void setData(WorkerData data);
    void setSettings(WorkerSettings settings);
};

#endif
