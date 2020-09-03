#ifndef WORKER_H
#define WORKER_H

#include "common.h"
#include "fifo.h"
#include <filesystem>
#include <string>

struct WorkerSettings
{
protected:
    Fifo queue_;
    pid_t pid_;

public:
    Fifo get_queue() const;
    void set_queue(Fifo queue);
    pid_t get_pid() const;
    void set_pid(pid_t pid);
};
struct WorkerData
{
    Records records;
    SummaryStats summary_stats;
};

struct Worker : WorkerSettings
{
private:
    WorkerData data_;
    Worker(){};

public:
    static Worker &get_instance()
    {
        static Worker instance;
        return instance;
    }
    Worker(Worker const &) = delete;
    void operator=(Worker const &) = delete;
    void Start();
    void Stop();
    WorkerData get_data() const;
    void set_data(WorkerData data);
    void set_settings(WorkerSettings settings);
};

#endif
