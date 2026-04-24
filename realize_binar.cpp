#include <vector>
#include <iostream>
#include <cstdint>
#include <string>
#include <ctime>
#include <cstring>
#include <stdexcept>
#include "info_binar.h"
#include <fstream>


#define PAGE_FILE_SIZE (16 + sizeof(Cell) * ELEM_SIZE)

FileSystem::FileSystem(const char* filename_, long sizeArry) {
  arraySize = sizeArry;
  pageCount = (arraySize + ELEM_SIZE - 1) / ELEM_SIZE;


  for (int i = 0; i < BUFF_SIZE; i++) {
    buffer[i].PageNum          = -1;
    buffer[i].flag_modification = 0;
    buffer[i].PageTime         = 0;
    memset(buffer[i].bitmap, 0, 16);
    memset(buffer[i].data,   0, sizeof(buffer[i].data));
  }

  file = fopen(filename_, "r+b");
  if (!file) {
    std::cout << "Бро создаём файл и.т.д кладём плитку и.т.п, а хотя не я же не мигрант. Впрочем можно больше и не делать ничего XD \n";

    file = fopen(filename_, "w+b");
    if (!file) {
      std::cout << "Ошибка: не удалось создать файл!\n";
      return;
    }

    FileHead header = {{'V', 'M'}, 'I', sizeArry};
    if (fwrite(&header, sizeof(header), 1, file) != 1) {
      std::cout << "Ошибка: не удалось записать заголовок файла!\n";
      fclose(file);
      file = nullptr;
      return;
    }


    std::vector<uint8_t> nuli(PAGE_FILE_SIZE, 0);
    for (long p = 0; p < pageCount; p++) {
      if (fwrite(nuli.data(), 1, PAGE_FILE_SIZE, file) != PAGE_FILE_SIZE) {
        std::cout << "Ошибка: не удалось инициализировать страницы файла!\n";
        fclose(file);
        file = nullptr;
        return;
      }
    }

    fflush(file);
  }

  for (int i = 0; i < BUFF_SIZE && i < pageCount; i++) {
    load_Page(i, i);
  }

  std::cout << "Файл, а кому не всё равно (открыт по идее) \n";
  std::cout << "Вообще я думал сюда вставить анекдот, но в моём арсенале только про говно и геев, поэтому увы и ах \n";
}

int FileSystem::get_elemFile(long id) {
  return id % ELEM_SIZE;
}

long FileSystem::get_PageNum(long id) {
  return id / ELEM_SIZE;
}

long FileSystem::page_offset(long Pagenum) {
  return sizeof(FileHead) + (Pagenum * PAGE_FILE_SIZE);
}

void FileSystem::write_PageFile(int slot) {
  if (buffer[slot].PageNum == -1) return;
  if (!buffer[slot].flag_modification) return;

  long offset = page_offset(buffer[slot].PageNum);
  if (fseek(file, offset, SEEK_SET) != 0) {
    std::cout << "Ошибка: не удалось позиционироваться в файле при записи!\n";
    return;
  }
  if (fwrite(buffer[slot].bitmap, 1, 16, file) != 16) {
    std::cout << "Ошибка: не удалось записать битовую карту на диск!\n";
    return;
  }
  if (fwrite(buffer[slot].data, sizeof(Cell), ELEM_SIZE, file) != (size_t)ELEM_SIZE) {
    std::cout << "Ошибка: не удалось записать данные страницы на диск!\n";
    return;
  }
  fflush(file);

  buffer[slot].flag_modification = 0;
}

void FileSystem::load_Page(int slot, long Pagenum) {
  long offset = page_offset(Pagenum);
  if (fseek(file, offset, SEEK_SET) != 0) {
    std::cout << "Ошибка: не удалось позиционироваться в файле при загрузке!\n";
    return;
  }
  if (fread(buffer[slot].bitmap, 1, 16, file) != 16) {
    std::cout << "Ошибка: не удалось прочитать битовую карту с диска!\n";
    return;
  }
  if (fread(buffer[slot].data, sizeof(Cell), ELEM_SIZE, file) != (size_t)ELEM_SIZE) {
    std::cout << "Ошибка: не удалось прочитать данные страницы с диска!\n";
    return;
  }

  buffer[slot].flag_modification = 0;
  buffer[slot].PageTime          = time(nullptr);
  buffer[slot].PageNum           = Pagenum;
}

int FileSystem::find_slot(long id) {
  long pageNum = get_PageNum(id);


  for (int i = 0; i < BUFF_SIZE; i++) {
    if (buffer[i].PageNum == pageNum) return i;
  }


  int oldest = 0;
  for (int i = 1; i < BUFF_SIZE; i++) {
    if (buffer[i].PageTime < buffer[oldest].PageTime) {
      oldest = i;
    }
  }


  if (buffer[oldest].flag_modification) {
    write_PageFile(oldest);
  }


  load_Page(oldest, pageNum);
  return oldest;
}

bool FileSystem::get_bit(uint8_t* bitmap, int pos) {
  return (bitmap[pos / 8] >> (pos % 8)) & 1;
}

bool FileSystem::set_bit(uint8_t* bitmap, int pos) {
  bitmap[pos / 8] |= (1 << (pos % 8));
  return true;
}

bool FileSystem::read(long id, int& value, std::vector<int>& history, int& fl_mod) {
  if (id < 0 || id >= arraySize) {
    throw std::out_of_range("Индекс вне диапазона");
  }

  int slot = find_slot(id);
  int off  = get_elemFile(id);

  Cell& cell = buffer[slot].data[off];

  if (!get_bit(buffer[slot].bitmap, off) || cell.count == 0) {
    value  = 0;
    fl_mod = 0;
    history.clear();
  } else {
    value  = cell.history[cell.count - 1];
    fl_mod = cell.flag_modification;
    history.assign(cell.history, cell.history + cell.count);
  }

  return true;
}

bool FileSystem::write(long id, int value) {
  if (id < 0 || id >= arraySize) {
    throw std::out_of_range("Индекс вне диапазона");
  }

  int slot = find_slot(id);
  int off  = get_elemFile(id);

  Cell& cell = buffer[slot].data[off];

  if (cell.count < HISTORY_SIZE) {
    cell.history[cell.count] = value;
    cell.count++;
  } else {
    for (int i = 0; i < HISTORY_SIZE - 1; i++) {
      cell.history[i] = cell.history[i + 1];
    }
    cell.history[HISTORY_SIZE - 1] = value;
  }

  cell.flag_modification = 1;
  set_bit(buffer[slot].bitmap, off);
  buffer[slot].flag_modification = 1;
  buffer[slot].PageTime = time(nullptr);

  return true;
}

FileSystem::~FileSystem() {
  for (int i = 0; i < BUFF_SIZE; i++) {
    write_PageFile(i);
  }

  if (file) {
    fclose(file);
    file = nullptr;
  }

  std::cout << "Файл закрыт теперь можно и отложить лабу на долгий срок, в отличии от светлого нефильтрованного\n";
}
