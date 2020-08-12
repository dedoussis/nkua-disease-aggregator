#ifndef IO_H
#define IO_H

#include <string>
#include <utility>
#include "common.h"

struct NamedPipe {
    private:
        const char* path;
        int bufferSize;

    public:
        NamedPipe(const char* p, int bs);
        ~NamedPipe();
        void send(std::string payload);
        std::pair<Command, std::string> receive();
};

#endif