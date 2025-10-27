#pragma once

#include <vector>
#include <string>

struct Point
{
    double x = 0;
    double y = 0;
};

struct GnuplotLine
{
    std::string name = "default_name";
    std::vector<Point> data;
    GnuplotLine() {}
    GnuplotLine(const std::string &name, const std::vector<Point> &src)
        : name(name), data(src) {}
    GnuplotLine(const std::string &name, const std::vector<double> x, const std::vector<double> &y);
};