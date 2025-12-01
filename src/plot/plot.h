#pragma once

#include <algorithm>

#include "plot/params.h"
#include "general/general.h"

std::vector<Point> merge(const std::vector<double> &x, const std::vector<double> &y);

int draw_plot(const GnuplotParams &p);
int draw_plot(const GnuplotMultiParams &p);

inline std::string endline_check(const std::string &title)
{
    int pos = title.find('\n');
    if (pos == std::string::npos)
        return title;
    return title.substr(0, pos);
}
inline std::string safe_path(const std::string head, const std::string &tail)
{
    return head + ((head.back() == '/') ? "" : "/") + tail;
}
inline std::string gnuplot_gen_data_file_name(const GnuplotParams &p)
{
    std::string res;
    for (const auto &line : p.lines)
    {
        res += line.name + "_";
    }
    std::ranges::replace(res, ' ', '_');
    return res;
}
inline std::string gnuplot_param(const std::string name, const std::string val, const std::string endline = "")
{
    return name + "=" + val + ";" + endline;
}
inline std::string gnuplot_param_str(const std::string name, const std::string val, const std::string endline = "")
{
    return name + "='" + val + "';" + endline;
}
template <typename ContainerType>
inline std::string gnuplot_long_param_str(const std::string name, ContainerType &&src, std::string endline = "")
{
    std::string result = name + "=[";
    for (auto &item : src)
        result += "'" + item + "', ";
    result.pop_back();
    result.back() = ']';
    return result + ";" + endline;
}
template <typename ContainerType>
inline std::string gnuplot_array(const std::string name, ContainerType &&src)
{
    std::string res =
        "array " + name + "[" + std::to_string(src.size()) + "] = [";
    for (auto &item : src)
        res += "'" + item + "', ";
    res.erase(res.end() - 2);
    res += "]";
    return res;
}
inline std::string gnuplot_var(const std::string name, const std::string val)
{
    return name + " = " + val;
}
inline std::string gnuplot_str(const std::string name, const std::string val)
{
    return name + " = '" + val + "'";
}

constexpr inline const char *gnuplot_line_prefix()
{
    return "gnuplot -p -e \"";
}
constexpr inline const char *gnuplot_line_suffix()
{
    return "\" plot_template.gp";
}
constexpr inline const char *gnuplot_line_multiplot_suffix()
{
    return "\" multiplot_template.gp";
}

inline std::string gnuplot_data_str(const Point &p)
{
    return std::to_string(p.x) + " " + std::to_string(p.y);
}
inline std::string gnuplot_extract_data_str(const GnuplotLine &line, const size_t &idx)
{
    return (idx < line.data.size()) ? gnuplot_data_str(line.data[idx]) : "NaN NaN";
}

inline int show_pic(const std::string &path, const std::string &pref = "plot/", const std::string &post = ".png")
{
    return exec("eog " + pref + "'" + path + "'" + post + " >/dev/null 2>&1 &");
}
inline auto show_pic(const GnuplotParams &p) { return show_pic(p.title); }
inline auto show_pic(const GnuplotMultiParams &p) { return show_pic(p.name); }