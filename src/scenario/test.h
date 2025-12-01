#pragma once

#include "modulation.h"
#include "plot/plot_line.h"
#include "noise/noise.h"
#include "gold.h"
#include "general/colors.h"

template <typename T>
inline std::string print(const std::vector<T> &src, const std::string name = "")
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
inline std::string to_string(std::vector<T> &src)
{
    std::string res;
    res.resize(src.size());
    for (size_t i = 0; i < res.size(); ++i)
        res[i] = src[i];
    return res;
}
template <typename T>
inline std::string bitseq_to_str(const std::vector<T> &src)
{
    std::string res;
    for (auto &x : src)
        res += std::to_string(x);
    return res;
}

inline std::string compare(const std::string &s1, const std::string &s2, bool &out)
{
    std::string res;
    size_t size = std::max(s1.size(), s2.size());
    out = 1;
    for (size_t i = 0; i < size; ++i)
    {
        if ((i < s1.size()) && (i < s2.size()))
        {
            if (s1[i] == s2[i])
            {
                res += s1[i];
                out = out & 1;
            }
            else
            {
                res += '-';
                out = 0;
            }
        }
        else
        {
            res += '-';
            out = 0;
        }
    }
    return res;
}

void test_scenario(const Config &conf)
{
    auto gold_mapper = create_mapper(
        conf,
        conf,
        {{0, 0}, {0, 1}, {1, 0}, {1, 1}});

    SourceParams sp = conf;
    std::string init_bits = bitseq_to_str(sp.bits);
    sp.bits = to_gold(sp.bits, gold_mapper);

    auto fm4 = generate_fm4(sp);
    auto fm4_n = apply_white_noise(fm4, conf);
    auto fm4_keys = generate_keys(sp);
    fm4_keys.resize(fm4_n.size());

    correlate_gold(fm4_n, gold_mapper);

    auto found_bits = find_bits(gold_mapper, sp, fm4_keys.size());
    std::string found_bits_str = bitseq_to_str(found_bits);
    bool out = 0;
    auto comp_str = compare(init_bits, found_bits_str, out);
    if (out)
        std::cout << GREEN << "Найденные биты совпали" << RESET << std::endl;
    else
        std::cout << RED << "Найденные биты не совпали" << RESET << std::endl;
    std::cout << "Исходные биты:  " << init_bits << std::endl
              << "Найденные биты: " << found_bits_str << std::endl
              << "Сравнение:      " << comp_str << std::endl;

    // графики
    auto fm4_l = line("Исходный сигнал", fm4_keys, fm4_n);

    GnuplotLineCmplx gm_l;
    for (auto &g : gold_mapper)
        gm_l.emplace_back(
            line(
                std::to_string(g.first.first) + std::to_string(g.first.second),
                fm4_keys,
                g.second.r));

    WindowParams w = conf;

    GnuplotParams plot;
    plot.gui_mode = w.windowing;
    plot.width = w.width;
    plot.height = w.height;

    auto plot_b = plot;
    plot_b.title = "Исследуемый сигнал";
    plot_b.x_label = "Время, с";
    plot_b.y_label = "Амплитуда сигнала";
    plot_b.lines = {fm4_l};

    auto plot_c = plot;
    plot_c.title = "Корреляция между сигналом и фильтрами";
    plot_b.x_label = "Время, с";
    plot_b.y_label = "Значение корреляции";
    plot_c.lines = gm_l;

    GnuplotMultiParams multiplot;
    multiplot.name = "Восстановление исходной информации путем согласованной фильтрации";
    multiplot.plots = {
        plot_b,
        plot_c,
    };

    if (w.single_window)
        for (auto &p : multiplot.plots)
            draw_plot(p);
    else
        draw_plot(multiplot);
}