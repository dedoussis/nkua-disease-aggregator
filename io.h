#ifndef IO_H
#define IO_H

#include <string>
#include <utility>
#include "common.h"

struct NamedPipe {
    private:
        const char* m_path;
        int m_bufferSize;

    public:
        NamedPipe(const char* path, int bufferSize);
        ~NamedPipe();
        void send(Command type, std::string payload);
        std::pair<Command, std::string> receive();
};

#endif