#include <vector>
#include <iostream>
#include <cstdint>
#include <cstdio>
#include <string>
#include <ctime>
#include "info_binar.h"
#include <fstream>


int main() {
  FileSystem* vm = nullptr;
  std::string cmd;
  std::cout << "Команды:\n"
  << "  Create <имя файла>\n"
  << "  Open <имя файла>\n"
  << "  Input(<индекс>, <значение>)\n"
  << "  Print(<индекс>)\n"
  << "  Help\n"
  << "  Exit\n";

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

      long idx; int val;
      sscanf(cmd.c_str(), "Input(%ld, %d)", &idx, &val);
      if (vm && vm->write(idx, val))
        std::cout << "Записано arr[" << idx << "] = " << val << "\n";
      else if (!vm)
        std::cout << "Ошибка: сначала создай или открой файл (Create / Open) или пресс HELP (а чё Г перевёрнута)\n";
      else
        std::cout << "Ошибка: индекс " << idx << " выходит за границы массива\n";

    } else if (cmd.substr(0, 5) == "Print") {

      long idx; int val;
      sscanf(cmd.c_str(), "Print(%ld)", &idx);
      if (vm && vm->read(idx, val))
        std::cout << "arr[" << idx << "] = " << val << "\n";
      else if (!vm)
        std::cout << "Ошибка: сначала создайте или откройте файл (Create / Open)\n";
      else
        std::cout << "Ошибка: индекс " << idx << " выходит за границы массива\n";

    } else if (cmd == "Help") {
      std::cout << "Команды:\n"
      << "  Create <имя файла>\n"
      << "  Open <имя файла>\n"
      << "  Input(<индекс>, <значение>)\n"
      << "  Print(<индекс>)\n"
      << "  Help\n"
      << "  Exit\n";

    }

    else if (cmd == "НЕГР") {
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
    }

    std::cout << "VM> ";
  }

  return 0;
}
