#pragma once

#include "gold.h"
#include "scheduler.h"
#include "scenario/restore.h"

class ExperimentParams
{
public:
    ExperimentParams() {}
    ExperimentParams(const Config &c)
    {
        auto e = c["Параметры исследования устойчивости"];
        noise_min = e["Минимальный уровень шума"].get<double>();
        noise_max = e["Максимальный уровень шума"].get<double>();
        noise_step_num = e["Количество шагов по уровню шума"].get<uint64_t>();
        exp_num = e["Число повторений генерирования сигналов"].get<uint64_t>();
        thread_num = e["Количество потоков"].get<uint64_t>();
    }

    double noise_min = 0;
    double noise_max = 0;
    uint64_t noise_step_num = 0;
    uint64_t exp_num = 0;
    uint64_t thread_num = 0;
};

uint64_t count_miss(const bitseq &l, const bitseq &r)
{
    auto size = std::min(l.size(), r.size());
    auto c = std::max(l.size(), r.size()) - size;
    for (size_t i = 0; i < size; ++i)
        if (l[i] != r[i])
            ++c;
    std::cerr << "c " << c << std::endl;
    return c;
}

double single_experiment(
    const Config &conf,
    std::map<BitPair, GoldHandler> &gm,
    std::mt19937 &rnd,
    const double &noise_level)
{
    SourceParams sp(conf, rnd);
    sp.noise = noise_level;

    auto bits = sp.bits;
    sp.bits = to_gold(sp.bits, gm);

    auto fm4 = generate_fm4(sp);
    auto fm4_n = apply_white_noise(fm4, sp);
    correlate_gold(fm4_n, gm);
    auto found_bits = find_bits(gm, sp, fm4_n.size());
    return double(count_miss(bits, found_bits)) / bits.size();
}

double experiment_sequence(
    const Config &conf,
    std::map<BitPair, GoldHandler> gm,
    std::mt19937 &rnd,
    const double &noise_level,
    const size_t &exp_num)
{
    double p = 0;
    for (size_t i = 0; i < exp_num; ++i)
        p += single_experiment(conf, gm, rnd, noise_level);
    return p / exp_num;
}

void test_scenario(const Config &conf)
{
    auto gold_mapper = create_mapper(
        conf,
        conf,
        {{0, 0}, {0, 1}, {1, 0}, {1, 1}});

    ExperimentParams ep = conf;

    Scheduler scheduler;
    scheduler.set_max_thread_num(ep.thread_num);

    auto rnd = create_random(ep.thread_num);

    double noise_step = (ep.noise_max - ep.noise_min) / double(ep.noise_step_num - 1);
    std::vector<double> result(ep.noise_step_num);
    std::vector<double> noise_levels(ep.noise_step_num);

    for (size_t i = 0; i < ep.noise_step_num; ++i)
    {
        double noise = ep.noise_min + i * noise_step;
        noise_levels[i] = noise;
        scheduler.start(
            [=, &rnd, &result]()
            {
                result[i] = experiment_sequence(
                    conf,
                    gold_mapper,
                    rnd[i % ep.thread_num],
                    noise,
                    ep.exp_num);
            });
    }

    std::cout << YELLOW << "Начато исследование устойчивости алгоритма" << RESET << std::endl;
    while (scheduler.get_done_task_counter() < ep.noise_step_num)
    {
        int progress = 100. * double(scheduler.get_done_task_counter()) / double(ep.noise_step_num);
        std::cout << "\r" << "Прогресс: " << progress << "%" << std::flush;
    }
    int progress = 100. * double(scheduler.get_done_task_counter()) / double(ep.noise_step_num);
    std::cout << "\r" << "Прогресс: " << progress << "%" << std::endl;
    std::cout << GREEN << "Завершено" << RESET << std::endl;

    // графики
    auto res_l = line("Вероятность ошибки", noise_levels, result);

    WindowParams w = conf;

    GnuplotParams plot;
    plot.gui_mode = w.windowing;
    plot.width = w.width;
    plot.height = w.height;

    auto plot_b = plot;
    plot_b.title = "График зависимости вероятности ошибки от SNR";
    plot_b.x_label = "Сигнал/шум, дБ";
    plot_b.y_label = "Вероятность ошибки";
    plot_b.lines = {res_l};

    GnuplotMultiParams multiplot;
    multiplot.name = "Восстановление исходной информации путем согласованной фильтрации";
    multiplot.plots = {
        plot_b,
    };

    if (w.single_window)
        for (auto &p : multiplot.plots)
            draw_plot(p);
    else
        draw_plot(multiplot);
}