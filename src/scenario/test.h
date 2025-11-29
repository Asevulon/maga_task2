#pragma once

#include "modulation.h"
#include "plot/plot_line.h"
#include "noise/noise.h"
#include "gold.h"

template <typename T>
std::string print(const std::vector<T> &src, const std::string name = "")
{
    std::stringstream ss;
    if (name.size())
        ss << name << ": ";
    for (auto &item : src)
        ss << item << ", ";
    auto res = ss.str();
    res.erase(res.begin() + res.size() - 2);
    return res;
}
template <typename T>
std::string to_string(std::vector<T> &src)
{
    std::string res;
    res.resize(src.size());
    for (size_t i = 0; i < res.size(); ++i)
        res[i] = src[i];
    return res;
}

void test_scenario(const Config &conf)
{
    auto gold_codes = create_gold(conf);
    for (auto &item : gold_codes)
        item.emplace_back(0);

    auto gold_mapper = create_mapper(
        {{0, 0}, {0, 1}, {1, 0}, {1, 1}},
        gold_codes);

    SourceParams sp = conf;
    sp.bits = to_gold(sp.bits, gold_mapper);

    auto fm4 = generate_fm4(sp);
    auto fm4_n = apply_white_noise(fm4, conf);
    auto fm4_keys = generate_keys(sp);

    auto fm4_l = nline(fm4_keys, fm4_n);

    WindowParams w = conf;

    GnuplotParams plot;
    plot.title = "bits";
    plot.lines = {fm4_l};
    plot.gui_mode = w.windowing;
    plot.width = w.width;
    plot.height = w.height;

    GnuplotMultiParams multiplot;
    multiplot.name = "test";
    multiplot.plots = {
        plot};

    if (w.single_window)
        for (auto &p : multiplot.plots)
            draw_plot(p);
    else
        draw_plot(multiplot);
}