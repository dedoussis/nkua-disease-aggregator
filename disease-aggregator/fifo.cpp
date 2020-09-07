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
#include <optional>

std::vector<std::byte> ReadChunks(int fd, int max_buffer_size, int length) {
  std::vector<std::byte> destination;
  int remaining_bytes = length + 1;
  while (remaining_bytes) {
    const int buffer_size =
        remaining_bytes < max_buffer_size ? remaining_bytes : max_buffer_size;
    std::vector<std::byte> chunk(buffer_size);

    const int read_bytes = read(fd, chunk.data(), buffer_size);
    if (!read_bytes)
      throw std::runtime_error("Length of file is less than expected");

    destination.insert(destination.end(), chunk.begin(), chunk.end());
    remaining_bytes -= read_bytes;
  }

  return destination;
}

void WriteChunks(int fd, std::vector<std::byte> source, int max_buffer_size,
                 int length) {
  size_t all_bytes = length + 1;
  int remaining_bytes = all_bytes;
  while (remaining_bytes) {
    const int buffer_size =
        remaining_bytes < max_buffer_size ? remaining_bytes : max_buffer_size;
    std::vector<std::byte>::const_iterator chunkStart =
        source.begin() + all_bytes - remaining_bytes;
    std::vector<std::byte>::const_iterator chunkEnd = chunkStart + buffer_size;
    const std::vector<std::byte> chunk =
        std::vector<std::byte>(chunkStart, chunkEnd);
    const int writen_bytes = write(fd, chunk.data(), buffer_size);
    if (!writen_bytes)
      throw std::runtime_error("Length of data is less than expected");

    remaining_bytes -= writen_bytes;
  }
}

struct Header {
  Command type;
  uint32_t length;
};

std::vector<std::byte> ToByteVector(std::string data) {
  std::vector<std::byte> payload_byte_vector;
  for (auto c : data)
    payload_byte_vector.push_back(std::byte(c));
  payload_byte_vector.push_back(std::byte('\0'));
  return payload_byte_vector;
}

std::vector<std::byte> ToByteVector(Header data) {
  auto data_ptr = reinterpret_cast<std::byte *>(&data);
  return std::vector<std::byte>(data_ptr, data_ptr + sizeof(data));
}

Fifo::Fifo(std::string path, int buffer_size) {
  path_ = path;
  buffer_size_ = buffer_size;
}

void Fifo::Make() { mkfifo(path_.c_str(), 0666); }

void Fifo::Destroy() { unlink(path_.c_str()); }

int Fifo::Open(mode_t mode) {
  return open(path_.c_str(), mode);
}

void Fifo::Enqueue(Command type, std::string payload, std::optional<int> fd) {
  Header header = {.type = type, .length = (uint32_t)payload.size()};

  int wfd = fd.has_value() ? fd.value() : Open(O_WRONLY);

  WriteChunks(wfd, ToByteVector(header), buffer_size_, sizeof(header));
  WriteChunks(wfd, ToByteVector(payload), buffer_size_, header.length);

  close(wfd);
}

std::pair<Command, std::string> Fifo::Dequeue(std::optional<int> fd) {  
  int rfd = fd.has_value() ? fd.value() : Open(O_RDONLY);

  std::vector<std::byte> headerBytes =
      ReadChunks(rfd, buffer_size_, sizeof(Header));
  Header *header = reinterpret_cast<Header *>(headerBytes.data());
  std::string payload =
      (char *)ReadChunks(rfd, buffer_size_, header->length).data();

  close(rfd);

  return std::make_pair(header->type, payload);
}
