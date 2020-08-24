#include "fifo.h"
#include "common.h"
#include <fcntl.h>
#include <iostream>
#include <stdexcept>
#include <stdio.h>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <utility>
#include <vector>

std::vector<std::byte> readChunks(int fd, int maxBufferSize, int length)
{
    std::vector<std::byte> destination;
    int remainingBytes = length + 1;
    while (remainingBytes)
    {
        const int bufferSize = remainingBytes < maxBufferSize ? remainingBytes : maxBufferSize;
        std::vector<std::byte> chunk(bufferSize);

        const int readBytes = read(fd, chunk.data(), bufferSize);
        if (!readBytes)
            throw std::runtime_error("Length of file is less than expected");

        destination.insert(destination.end(), chunk.begin(), chunk.end());
        remainingBytes -= readBytes;
    }

    return destination;
}

void writeChunks(int fd, std::vector<std::byte> source, int maxBufferSize, int length)
{
    size_t allBytes = length + 1;
    int remainingBytes = allBytes;
    while (remainingBytes)
    {
        const int bufferSize = remainingBytes < maxBufferSize ? remainingBytes : maxBufferSize;
        std::vector<std::byte>::const_iterator chunkStart = source.begin() + allBytes - remainingBytes;
        std::vector<std::byte>::const_iterator chunkEnd = chunkStart + bufferSize;
        const std::vector<std::byte> chunk = std::vector<std::byte>(chunkStart, chunkEnd);
        const int writenBytes = write(fd, chunk.data(), bufferSize);
        if (!writenBytes)
            throw std::runtime_error("Length of data is less than expected");

        remainingBytes -= writenBytes;
    }
}

struct Header
{
    Command type;
    uint32_t length;
};

std::vector<std::byte> toByteVector(std::string data)
{
    std::vector<std::byte> payloadByteVector;
    for (auto c : data)
        payloadByteVector.push_back(std::byte(c));
    payloadByteVector.push_back(std::byte('\0'));
    return payloadByteVector;
}

std::vector<std::byte> toByteVector(Header data)
{
    auto dataPtr = reinterpret_cast<std::byte *>(&data);
    return std::vector<std::byte>(dataPtr, dataPtr + sizeof(data));
}

Fifo::Fifo(std::string path, int bufferSize)
{
    m_path = path;
    m_bufferSize = bufferSize;
}

void Fifo::make()
{
    mkfifo(m_path.c_str(), 0666);
}

void Fifo::destroy()
{
    unlink(m_path.c_str());
}

void Fifo::enqueue(Command type, std::string payload)
{
    Header header = {.type = type, .length = (uint32_t)payload.size()};

    int fd = open(m_path.c_str(), O_WRONLY);

    writeChunks(fd, toByteVector(header), m_bufferSize, sizeof(header));

    if (header.length)
        writeChunks(fd, toByteVector(payload), m_bufferSize, header.length);

    close(fd);
}

std::pair<Command, std::string> Fifo::dequeue()
{
    int fd = open(m_path.c_str(), O_RDONLY);
    std::vector<std::byte> headerBytes = readChunks(fd, m_bufferSize, sizeof(Header));
    Header *header = reinterpret_cast<Header *>(headerBytes.data());

    if (!header->length)
        return std::make_pair(header->type, std::string());

    std::string payload = (char *)readChunks(fd, m_bufferSize, header->length).data();

    close(fd);

    return std::make_pair(header->type, payload);
}
