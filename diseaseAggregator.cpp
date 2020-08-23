#include "aggregator.h"
#include "args.h"
#include "common.h"
#include "fifo.h"
#include "worker.h"
#include "external.h"
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <variant>

using namespace std;

External::SummaryStatisticsRequest generateSummaryStatisticsRequest(string inputDir)
{
    External::SummaryStatisticsRequest request;
    for (auto &dirEntry : filesystem::directory_iterator(inputDir))
    {
        if (filesystem::is_directory(dirEntry))
            request.filePaths.push_back(filesystem::path(dirEntry));
    }
    return request;
}

int startCommandInterface(Aggregator aggregator, External::Request initialRequest)
{
    int inputCounter = 0;
    Command command = Command::SummaryStatistics;
    External::Request request = initialRequest;
    while (command != Command::Exit)
    {
        if (inputCounter)
        {
            cout << "Enter your command: ";
            string inputString;
            getline(cin, inputString);
            try
            {
                tie(command, request) = parseCommand(inputString);
            }
            catch (const runtime_error &error)
            {
                cerr << error.what() << endl;
                continue;
            }
        }

        External::Response response = visit(aggregator, request);

        cout << External::serialize(response) << endl;
        inputCounter++;
    }

    return 0;
}

int main(int argc, char *argv[])
{
    InitialArgs args = parseInitialArgs(argc, argv);

    External::Request initialRequest = generateSummaryStatisticsRequest(args.inputDir);
    Aggregator aggregator;

    for (int i = 0; i < args.numWorkers; ++i)
    {
        Fifo queue("pipe_" + to_string(i), args.bufferSize);
        Worker worker;
        worker.setQueue(queue);
        worker.getQueue().make();
        if (pid_t pid = fork())
        {
            worker.setPid(pid);
            aggregator.addWorker(worker);
        }
        else
        {
            worker.setPid(getpid());
            worker.start();
            return 0;
        }
    }

    startCommandInterface(aggregator, initialRequest);

    return 0;
}
