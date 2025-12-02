#include <iostream>

#include "scenario/scenario.h"

int main(int argc, char *argv[])
{
    try
    {
        std::cerr << "Running program: " << argv[0] << std::endl;

        auto conf = load_config("config.json");
        std::cerr << "Config: " << conf << std::endl;

        std::string mode = conf["Режим запуска"];
        if (mode == "Восстановление_исходной_информации")
            restore_scenario(conf);
        else if (mode == "Исследование_устойчивости")
            test_scenario(conf);
        else
        {
            std::cout << RED << "Неизвестный режим запуска: " << mode << RESET << std::endl;
            std::cout << "Возможные режимы запуска: ";
            std::vector<std::string> modes = conf["Возможные режимы запуска"];
            std::string modes_str;
            for (auto &m : modes)
                modes_str += m + ", ";
            modes_str.pop_back();
            std::cout << modes_str << std::endl;
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }

    return 0;
}