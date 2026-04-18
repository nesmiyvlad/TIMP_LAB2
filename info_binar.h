#ifndef INFO_BINAR_H
#define INFO_BINAR_H

#include <fstream>
#include <vector>
#include <iostream>
#include <cstdint>
#include <string>
#include <ctime>
#include <cstdio>

#define PAGE_SIZE 512
#define BUFF_SIZE 3
#define ELEM_SIZE 128

#pragma pack(push,1)

struct Sheet {
  time_t PageTime;
  long PageNum;
  int flag_modification;
  uint8_t bitmap[16];
  int data[ELEM_SIZE];
};

struct FileHead {
  char signature[2];
  char type;
  long SizeArr;
};

#pragma pack(pop)

class FileSystem {
private:
  FILE* file;
  long arraySize;
  long pageCount;
  Sheet buffer[BUFF_SIZE];
public:
  int get_elemFile(long id);
  long get_PageNum(long id);
  long page_offset(long Pagenum);

  void write_PageFile(int slot);
  void load_Page(int slot, long Pagenum);

  int find_slot(long id);

  bool get_bit(uint8_t* bitmap, int pos);
  bool set_bit(uint8_t* bitmap, int pos);

  bool write(long id, int value);
  bool read(long id, int& value);

  void help();
  FileSystem(const char* filename_, long sizeArry);
  ~FileSystem();
};

#endif
