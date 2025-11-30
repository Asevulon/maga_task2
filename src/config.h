#pragma once
#include <nlohmann/json.hpp>
#include <string>
#include <fstream>

typedef nlohmann::json Config;

inline Config load_config(std::string path)
{
    std::ifstream file(path);
    Config conf;
    file >> conf;
    return conf;
}

class WindowParams
{
public:
    std::string windowing;
    std::string width;
    std::string height;
    bool single_window = false;

    WindowParams() {}
    WindowParams(const Config &c)
    {
        auto w = c["Отображение"];
        windowing = w["В окне"];
        width = w["Ширина изображения"];
        height = w["Высота изображения"];
        single_window = w["Отдельное окно для каждого графика"];
    }
};
