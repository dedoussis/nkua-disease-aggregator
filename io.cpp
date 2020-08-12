#include <stdio.h> 
#include <string> 
#include <fcntl.h> 
#include <sys/stat.h> 
#include <sys/types.h> 
#include <unistd.h>
#include <vector>
#include <utility>
#include "io.h"
#include "common.h"

struct Header {
    Command type;
    uint32_t length;
};

NamedPipe::NamedPipe(const char* p, int bs)
{
    
    Header header;
    path = p;
    bufferSize = bs - sizeof(header);
    mkfifo(path, 0666);
}

NamedPipe::~NamedPipe()
{
    unlink(path);
}

void NamedPipe::send(std::string payload)
{
    int fd = open(path, O_WRONLY);
    write(fd, payload.c_str(), payload.size() + 1);
    close(fd);
}

std::pair<Command, std::string> NamedPipe::receive()
{
    std::vector<char> buffer(bufferSize);
    int rfd = open(path, O_RDONLY);
    read(rfd, buffer.data(), buffer.size());
    close(rfd);
    return std::make_pair(Command::DiseaseFrequency, std::string(buffer.data()));
}
