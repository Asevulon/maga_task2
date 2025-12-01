#include "plot/plot.h"
#include "general/general.h"

#include <fstream>
#include <sstream>
#include "plot.h"
#include <algorithm>
#include <ranges>
#include <cstdint>

using std::ranges::for_each;

std::vector<Point> merge(const std::vector<double> &x, const std::vector<double> &y)
{
    std::vector<Point> res;

    size_t size = std::min(x.size(), y.size());
    res.resize(size);

    for (uint64_t i = 0; i < size; ++i)
    {
        res[i].x = x[i];
        res[i].y = y[i];
    }

    return res;
}

void print_plot_data(const GnuplotParams &p)
{
    std::string data_path = safe_path(p.data_dir, p.title);

    std::ofstream file(data_path);

    size_t max_size = 0;

    std::string header_line;

    for (const auto &line : p.lines)
    {
        header_line +=
            "\"x_" + line.name + "\" " + "\"" + line.name + "\" ";

        if (max_size < line.data.size())
            max_size = line.data.size();
    }
    header_line.back() = '\n';
    file << header_line;

    for (size_t i = 0; i < max_size; ++i)
    {
        std::string data_line;
        for (const auto &line : p.lines)
        {
            data_line += gnuplot_extract_data_str(line, i) + " ";
        }
        data_line.back() = '\n';
        file << data_line;
    }
    file.close();
}

int draw_plot(const GnuplotParams &p)
{
    print_plot_data(p);
    return exec(p.cmd_line());
}

int draw_plot(const GnuplotMultiParams &p)
{
    for (auto &p : p.plots)
        print_plot_data(p);
    return (exec(p.cmd_line()));
}

std::string GnuplotParams::cmd_line() const
{
    string out_name = endline_check(title) + out_ext;

    string data_path = safe_path(data_dir, title);
    string out_path = safe_path(out_dir, out_name);

    std::stringstream line;
    line << gnuplot_line_prefix()
         << gnuplot_param_str("datafile", data_path, " ")
         << gnuplot_param_str("output_file", out_path, " ")
         << gnuplot_param_str("custom_title", title, " ")
         << gnuplot_param("width", width, " ")
         << gnuplot_param("height", height, " ")
         << gnuplot_param_str("x_label", x_label, " ")
         << gnuplot_param_str("y_label", y_label, " ")
         << gnuplot_param_str("gui_mode", gui_mode)
         << gnuplot_line_suffix();
    return line.str();
}

std::string GnuplotMultiParams::cmd_line() const
{
    using std::string;
    using std::vector;

    string safe_name = endline_check(name);
    string out_name = safe_name + out_ext;
    string out_path = safe_path(out_dir, out_name);
    string out_script_path = safe_path(out_script_dir, safe_name) + ".gp";

    vector<string> datafile;
    vector<string> custom_title;
    vector<string> x_label;
    vector<string> y_label;

    for (auto &p : plots)
    {
        string data_path = safe_path(p.data_dir, p.title);

        datafile.emplace_back(data_path);
        custom_title.emplace_back(p.title);
        x_label.emplace_back(p.x_label);
        y_label.emplace_back(p.y_label);
    }

    std::ofstream out_script_file(out_script_path);
    out_script_file
        << gnuplot_str_array("datafiles", datafile) << std::endl
        << gnuplot_str("output_file", out_path) << std::endl
        << gnuplot_str_array("titles", custom_title) << std::endl
        << gnuplot_var("width", width) << std::endl
        << gnuplot_var("height", height) << std::endl
        << gnuplot_str_array("xlabels", x_label) << std::endl
        << gnuplot_str_array("ylabels", y_label) << std::endl
        << gnuplot_str("gui_mode", gui_mode) << std::endl
        << gnuplot_str("window_title", name) << std::endl;
    out_script_file.close();

    std::stringstream cmd_line;
    cmd_line
        << gnuplot_line_prefix()
        << gnuplot_param_str("params_file", out_script_path)
        << gnuplot_line_multiplot_suffix();

    return cmd_line.str();
}

GnuplotLine::GnuplotLine(const std::string &name, const std::vector<double> x, const std::vector<double> &y)
    : name(name)
{
    data = merge(x, y);
}
