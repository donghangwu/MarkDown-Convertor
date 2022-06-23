#include "fs_wrapper.h"

OFStream::OFStream(std::string file_path) {
  file_stream = new std::ofstream(file_path, std::ios::out);
}

void OFStream::write(std::string str) {
  *file_stream<<str;
}

bool OFStream::is_open() {
  return file_stream->is_open();
}

void OFStream::close() {
  return file_stream->close();
}

IFStream::IFStream(std::string file_path) {
  file_stream = new std::ifstream(file_path);
}

bool IFStream::is_open() {
  return file_stream->is_open();
}

void IFStream::close() {
  return file_stream->close();
}

bool IFStream::get(char &ch) {
  if (file_stream->get(ch)) {
    return true;
  }
  return false;
}

