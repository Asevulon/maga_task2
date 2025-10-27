#pragma once

#include "modulation.h"
#include "plot/plot_line.h"

void test_scenario(const Config &conf)
{
    auto fm4 = generate_fm4(conf);
    auto fm4_keys = generate_keys(conf);

    auto fm4_l = nline(fm4_keys, fm4);

    GnuplotParams plot;
    plot.title = "fm4";
    plot.lines = {fm4_l};

    GnuplotMultiParams multiplot;
    multiplot.name = "test";
    multiplot.plots = {
        plot,
        plot,
        plot};

    // draw_plot(plot);
    draw_plot(multiplot);
}