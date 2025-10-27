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
