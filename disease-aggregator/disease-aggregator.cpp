#include "aggregator.h"
#include "args.h"
#include "common.h"
#include "external.h"
#include "fifo.h"
#include "worker.h"
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <unistd.h>
#include <variant>

using namespace std;

External::SummaryStatisticsRequest GenerateSummaryStatisticsRequest(string input_dir)
{
    External::SummaryStatisticsRequest request;
    for (auto &dir_entry : filesystem::directory_iterator(input_dir))
    {
        if (filesystem::is_directory(dir_entry))
            request.file_paths.push_back(filesystem::path(dir_entry));
    }
    return request;
}

int StartCommandInterface(Aggregator aggregator, External::Request initial_request)
{
    Command command = Command::kSummaryStatistics;
    External::Request request = initial_request;
    while (command != Command::kExit)
    {
        if (aggregator.IsInitialised())
        {
            cout << "Enter your command: ";
            string input_string;
            getline(cin, input_string);
            try
            {
                tie(command, request) = ParseCommand(input_string);
            }
            catch (const runtime_error &error)
            {
                cerr << error.what() << endl;
                continue;
            }
        }

        External::Response response = visit(aggregator, request);

        cout << External::Serialize(response) << endl;
    }

    return 0;
}

int main(int argc, char *argv[])
{
    InitialArgs args = ParseInitialArgs(argc, argv);

    External::Request initial_request = GenerateSummaryStatisticsRequest(args.input_dir);
    Aggregator aggregator;

    for (int i = 0; i < args.num_workers; ++i)
    {
        Fifo queue("pipe_" + to_string(i), args.buffer_size);
        WorkerSettings worker_settings;
        worker_settings.set_queue(queue);
        worker_settings.get_queue().Make();
        if (pid_t pid = fork())
        {
            worker_settings.set_pid(pid);
            aggregator.add_worker(worker_settings);
        }
        else
        {
            worker_settings.set_pid(getpid());
            Worker &worker = Worker::get_instance();
            worker.set_settings(worker_settings);
            worker.Start();
            return 0;
        }
    }

    StartCommandInterface(aggregator, initial_request);

    return 0;
}
