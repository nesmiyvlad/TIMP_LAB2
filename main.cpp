#include <vector>
#include <iostream>
#include <cstdint>
#include <cstdio>
#include <string>
#include <ctime>
#include "info_binar.h"
#include <fstream>

// Печатаем список команд
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
  char inputType = 'I'; // текущий тип ввода: 'I' = int, 'C' = char

  printHelp();
  std::cout << "Текущий тип ввода: int\n";
  std::cout << "VM> ";

  while (std::getline(std::cin, cmd)) {

    if (cmd.substr(0, 6) == "Create") {

      std::string fname = cmd.substr(7);
      if (vm) delete vm;
      vm = new FileSystem(fname.c_str(), 50000);

    } else if (cmd.substr(0, 4) == "Open") {

      std::string fname = cmd.substr(5);
      if (vm) delete vm;
      vm = new FileSystem(fname.c_str(), 50000);

    } else if (cmd.substr(0, 5) == "Input") {

      if (!vm) {
        std::cout << "Ошибка: сначала создай или открой файл (Create / Open) или пресс HELP (а чё Г перевёрнута)\n";

      } else if (inputType == 'I') {
        // Режим int: Input(42, 999)
        long idx; int val;
        int parsed = sscanf(cmd.c_str(), "Input(%ld, %d)", &idx, &val);
        if (parsed != 2) {
          std::cout << "Ошибка: неверный формат. Пример: Input(42, 999)\n";
        } else if (vm->write(idx, val)) {
          std::cout << "Записано arr[" << idx << "] = " << val << "\n";
        } else {
          std::cout << "Ошибка: индекс " << idx << " выходит за границы массива\n";
        }

      } else {
        // Режим char: Input(42, g)
        long idx; char ch;
        int parsed = sscanf(cmd.c_str(), "Input(%ld, %c)", &idx, &ch);
        if (parsed != 2) {
          std::cout << "Ошибка: неверный формат. Пример: Input(42, g)\n";
        } else if (vm->write(idx, (int)ch)) {
          std::cout << "Записано arr[" << idx << "] = " << ch << "\n";
        } else {
          std::cout << "Ошибка: индекс " << idx << " выходит за границы массива\n";
        }
      }

    } else if (cmd.substr(0, 5) == "Print") {

      if (!vm) {
        std::cout << "Ошибка: сначала создай или открой файл (Create / Open) или пресс HELP (а чё Г перевёрнута)\n";
      } else {
        long idx; int val;
        sscanf(cmd.c_str(), "Print(%ld)", &idx);
        if (vm->read(idx, val)) {
          if (inputType == 'I') {
            std::cout << "arr[" << idx << "] = " << val << "\n";
          } else {
            // Если тип char — печатаем как символ
            std::cout << "arr[" << idx << "] = " << (char)val << "\n";
          }
        } else {
          std::cout << "Ошибка: индекс " << idx << " выходит за границы массива\n";
        }
      }

    } else if (cmd.substr(0, 6) == "Change") {

      // Парсим тип из команды: Change (int/char)
      std::string typeStr;
      // Ищем скобку и берём что внутри
      size_t open  = cmd.find('(');
      size_t close = cmd.find(')');
      if (open == std::string::npos || close == std::string::npos) {
        std::cout << "Ошибка: неверный формат. Пример: Change (int/char)\n";
      } else {
        typeStr = cmd.substr(open + 1, close - open - 1);
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
    }

    else if (cmd == "Exit") {
      delete vm;
      vm = nullptr;
      break;
    }
    else if (cmd == "Бабло") {
      std::cout << "  Сказано же в разработке\n";
    }

    std::cout << "VM> ";
  }

  return 0;
}
