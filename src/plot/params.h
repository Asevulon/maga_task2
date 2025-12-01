#pragma once

#include "plot/types.h"

class GnuplotParams
{
public:
    using string = std::string;
    string data_dir = "data/";
    string out_dir = "plot/";
    string title = "title";
    string width = "1280";
    string height = "800";
    string x_label = "X";
    string y_label = "Y";
    string out_ext = ".png";
    string gui_mode = "1";

    std::vector<GnuplotLine> lines;

    virtual std::string cmd_line() const;
};

class GnuplotMultiParams
{
public:
    using string = std::string;
    string width = "1920";
    string height = "1280";
    string name = "default";
    string out_ext = ".png";
    string out_dir = "plot/";
    string out_script_dir = "gnuplot_out_script/";
    string gui_mode = "1";

    std::vector<GnuplotParams> plots;

    virtual std::string cmd_line() const;
};
