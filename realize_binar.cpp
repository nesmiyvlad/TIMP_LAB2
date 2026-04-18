#include <vector>
#include <iostream>
#include <cstdint>
#include <string>
#include <ctime>
#include <cstring>
#include "info_binar.h"
#include <fstream>

FileSystem::FileSystem(const char* filename_, long sizeArry) {
  arraySize = sizeArry;
  pageCount = (arraySize * sizeof(int) + PAGE_SIZE - 1) / PAGE_SIZE;

  // Сначала инициализируем буфер — до любых load_Page вызовов
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
    fwrite(&header, sizeof(header), 1, file);

    uint8_t nuli[16 + PAGE_SIZE] = {};
    for (long p = 0; p < pageCount; p++) {
      fwrite(nuli, 1, sizeof(nuli), file);
    }
    // Сбрасываем буфер записи на диск
    fflush(file);
  }

  // Загружаем первые BUFF_SIZE страниц в буфер
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
  return sizeof(FileHead) + (Pagenum * (16 + PAGE_SIZE));
}

void FileSystem::write_PageFile(int slot) {
  // Не пишем пустые слоты (PageNum == -1)
  if (buffer[slot].PageNum == -1) return;
  if (!buffer[slot].flag_modification) return;

  long offset = page_offset(buffer[slot].PageNum);
  fseek(file, offset, SEEK_SET);

  fwrite(buffer[slot].bitmap, 1, 16, file);
  fwrite(buffer[slot].data, sizeof(int), ELEM_SIZE, file);
  fflush(file); // гарантируем что данные реально ушли на диск

  buffer[slot].flag_modification = 0;
}

void FileSystem::load_Page(int slot, long Pagenum) {
  long offset = page_offset(Pagenum);
  fseek(file, offset, SEEK_SET);

  fread(buffer[slot].bitmap, 1, 16, file);
  fread(buffer[slot].data, sizeof(int), ELEM_SIZE, file);

  buffer[slot].flag_modification = 0;
  buffer[slot].PageTime          = time(nullptr);
  buffer[slot].PageNum           = Pagenum;
}

int FileSystem::find_slot(long id) {
  long pageNum = get_PageNum(id);

  // 1. Ищем страницу которая уже в буфере
  for (int i = 0; i < BUFF_SIZE; i++) {
    if (buffer[i].PageNum == pageNum) return i;
  }

  // 2. Страницы нет — ищем самый старый слот для вытеснения
  int oldest = 0;
  for (int i = 1; i < BUFF_SIZE; i++) {
    if (buffer[i].PageTime < buffer[oldest].PageTime) {
      oldest = i;
    }
  }

  // 3. Если слот был изменён — сначала сохраняем на диск
  if (buffer[oldest].flag_modification) {
    write_PageFile(oldest);
  }

  // 4. Загружаем нужную страницу
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

bool FileSystem::read(long id, int& value) {
  if (id < 0 || id >= arraySize) return false;

  int slot = find_slot(id);
  int off  = get_elemFile(id);

  if (!get_bit(buffer[slot].bitmap, off)) {
    value = 0;
  } else {
    value = buffer[slot].data[off];
  }

  return true;
}

bool FileSystem::write(long id, int value) {
  if (id < 0 || id >= arraySize) return false;

  int slot = find_slot(id);
  int off  = get_elemFile(id);

  buffer[slot].data[off] = value;
  set_bit(buffer[slot].bitmap, off);

  buffer[slot].flag_modification = 1;
  buffer[slot].PageTime          = time(nullptr);

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
