#include <iostream>

#include "scenario/scenario.h"

int main(int argc, char *argv[])
{
    try
    {
        std::cerr << "Running program: " << argv[0] << std::endl;

        auto conf = load_config("config.json");
        std::cerr << "Config: " << conf << std::endl;

        test_scenario(conf);
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }

    return 0;
}