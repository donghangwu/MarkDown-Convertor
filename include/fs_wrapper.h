#ifndef FS_WRAPPER_H
#define FS_WRAPPER_H
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <fstream>

class OFSWrapper {
 public:
  virtual bool is_open() = 0;
  virtual void close() = 0;
  virtual void write(std::string str) = 0;
};

class OFStream : public OFSWrapper {
 public:
  OFStream(std::string file_path);
  bool is_open();
  void close();

  void write(std::string str);

 private:
  std::ofstream *file_stream;
};

class IFSWrapper {
 public:
  virtual bool is_open() = 0;
  virtual void close() = 0;
  virtual bool get(char &ch) = 0;
};

class IFStream : public IFSWrapper {
 public:
  IFStream(std::string file_path);
  bool is_open();
  void close();
  bool get(char &ch);

 private:
  std::ifstream *file_stream;
};
#endif