#include <vector>
#include <iostream>
#include <cstdint>
#include <cstdio>
#include <string>
#include <ctime>
#include "info_binar.h"
#include <fstream>


void printHelp() {
  std::cout << "Команды:\n"
  << "  Create <имя файла>\n"
  << "  Open <имя файла>\n"
  << "  Input(<индекс>, <значение>)\n"
  << "  Print(<индекс>)\n"
  << "  Change (int/char) - изменить тип вводимого значения\n"
  << "  Help\n"
  << "  Exit\n";
}

int main() {
  FileSystem* vm      = nullptr;
  std::string cmd;
  char inputType = 'I';

  printHelp();
  std::cout << "Текущий тип ввода: int\n";
  std::cout << "VM> ";

  while (std::getline(std::cin, cmd)) {

    if (cmd.empty()) {
      std::cout << "VM> ";
      continue;
    }

    if (cmd.substr(0, 6) == "Create") {

      if (cmd.size() <= 7) {
        std::cout << "Ошибка: не указано имя файла. Пример: Create myfile.vm\n";
      } else {
        std::string fname = cmd.substr(7);
        if (vm) delete vm;
        vm = new FileSystem(fname.c_str(), 50000);
      }

    } else if (cmd.substr(0, 4) == "Open") {

      if (cmd.size() <= 5) {
        std::cout << "Ошибка: не указано имя файла. Пример: Open myfile.vm\n";
      } else {
        std::string fname = cmd.substr(5);
        if (vm) delete vm;
        vm = new FileSystem(fname.c_str(), 50000);
      }

    } else if (cmd.substr(0, 5) == "Input") {

      if (!vm) {
        std::cout << "Ошибка: сначала создай или открой файл (Create / Open) или пресс HELP (а чё Г перевёрнута)\n";

      } else if (inputType == 'I') {

        long idx; int val;
        int parsed = sscanf(cmd.c_str(), "Input(%ld, %d)", &idx, &val);
        if (parsed != 2) {
          std::cout << "Ошибка: неверный формат. Пример: Input(42, 999)\n";
        } else {
          try {
            vm->write(idx, val);
            int val2, fl; std::vector<int> hist;
            vm->read(idx, val2, hist, fl);
            std::cout << "Записано arr[" << idx << "] = [";
            for (size_t i = 0; i < hist.size(); i++) {
              std::cout << hist[i];
              if (i + 1 < hist.size()) std::cout << ", ";
            }
            std::cout << "]  fl_mod = " << fl << "\n";
          } catch (const std::out_of_range& e) {
            std::cout << "Исключение (out_of_range): " << e.what() << "\n";
          }
        }

      } else {
        long idx; char ch;
        int parsed = sscanf(cmd.c_str(), "Input(%ld, %c)", &idx, &ch);
        if (parsed != 2) {
          std::cout << "Ошибка: неверный формат. Пример: Input(42, g)\n";
        } else {
          try {
            vm->write(idx, (int)ch);        // бросает throw если индекс за границей
            int val2, fl; std::vector<int> hist;
            vm->read(idx, val2, hist, fl);
            std::cout << "Записано arr[" << idx << "] = [";
            for (size_t i = 0; i < hist.size(); i++) {
              std::cout << (char)hist[i];
              if (i + 1 < hist.size()) std::cout << ", ";
            }
            std::cout << "]  fl_mod = " << fl << "\n";
          } catch (const std::out_of_range& e) {  // ловим исключение
            std::cout << "Исключение (out_of_range): " << e.what() << "\n";
          }
        }
      }

    } else if (cmd.substr(0, 5) == "Print") {

      if (!vm) {
        std::cout << "Ошибка: сначала создай или открой файл (Create / Open) или пресс HELP (а чё Г перевёрнута)\n";
      } else {
        long idx;
        if (sscanf(cmd.c_str(), "Print(%ld)", &idx) != 1) {
          std::cout << "Ошибка: неверный формат. Пример: Print(42)\n";
        } else {
          try {
            int val, fl; std::vector<int> hist;
            vm->read(idx, val, hist, fl);   // <- бросает throw если индекс за границей
            std::cout << "arr[" << idx << "] = [";
            if (hist.empty()) {
              std::cout << "пусто";
            } else {
              for (size_t i = 0; i < hist.size(); i++) {
                if (inputType == 'I') std::cout << hist[i];
                else                  std::cout << (char)hist[i];
                if (i + 1 < hist.size()) std::cout << ", ";
              }
            }
            std::cout << "]  fl_mod = " << fl << "\n";
          } catch (const std::out_of_range& e) {  // ловим исключение
            std::cout << "Исключение (out_of_range): " << e.what() << "\n";
          }
        }
      }

    } else if (cmd.substr(0, 6) == "Change") {

      size_t open  = cmd.find('(');
      size_t close = cmd.find(')');
      if (open == std::string::npos || close == std::string::npos) {
        std::cout << "Ошибка: неверный формат. Пример: Change (int/char)\n";
      } else {
        std::string typeStr = cmd.substr(open + 1, close - open - 1);
        if (typeStr == "int") {
          if (inputType == 'I') {
            std::cout << "Тип уже установлен: int\n";
          } else {
            inputType = 'I';
            std::cout << "Тип с char изменён на int\n";
          }
        } else if (typeStr == "char") {
          if (inputType == 'C') {
            std::cout << "Тип уже установлен: char\n";
          } else {
            inputType = 'C';
            std::cout << "Тип с int изменён на char\n";
          }
        } else {
          std::cout << "Ошибка: неизвестный тип \"" << typeStr << "\". Доступно: int, char\n";
        }
      }

    } else if (cmd == "Help") {
      printHelp();
      std::cout << "Текущий тип ввода: " << (inputType == 'I' ? "int" : "char") << "\n";

    } else if (cmd == "НЕГР") {
      std::cout << "А всё нормас Г вернули в нормальный вид. IQ больше чем у топовый подросток😈😈😈😈 \n";
      std::cout << "Команды:\n"
      << "  Create <имя файла>\n"
      << "  Open <имя файла>\n"
      << "  Input(<индекс>, <значение>)\n"
      << "  Print(<индекс>)\n"
      << "  Help\n"
      << "  Exit\n"
      << "  Кнопка Бабло (В разработке)\n";

    } else if (cmd == "Exit") {
      delete vm;
      vm = nullptr;
      break;

    } else if (cmd == "Бабло") {
      std::cout << "  Сказано же в разработке\n";

    } else {
      std::cout << "Ошибка: неизвестная команда \"" << cmd << "\". Введите Help для списка команд\n";
    }

    std::cout << "VM> ";
  }

  return 0;
}
