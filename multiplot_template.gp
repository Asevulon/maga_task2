if (exists("params_file")) {
    load params_file
} else {
    datafiles = ["data.txt"]
    titles = ["default_title"]
    xlabels = ["X"]
    ylabels = ["Y"]
    output_file = "graph.png"
    width = 800
    height = 600
    gui_mode = 0
}

N = |datafiles|

if (|titles| < N) {
    do for [i = |titles|+1:N] {
        titles = titles._["default_title"]
    }
}
if (|xlabels| < N) {
    do for [i = |xlabels|+1:N] {
        xlabels = xlabels._["X"]
    }
}
if (|ylabels| < N) {
    do for [i = |ylabels|+1:N] {
        ylabels = ylabels._["Y"]
    }
}

if (gui_mode) {
    set terminal qt size width, height
    unset mouse
} else {
    set terminal pngcairo size width, height font "Sans,12"
    set output output_file
}

set multiplot layout N,1 rowsfirst

do for [i = 1:N] {
    df   = datafiles[i]
    tit  = titles[i]
    xl   = xlabels[i]
    yl   = ylabels[i]

    set title tit
    set xlabel xl
    set ylabel yl
    set grid
    set key outside top left
    set offsets 0, 0, 0.1, 0.1

    stats df skip 1 nooutput
    ncols = STATS_columns
    nlines = int(ncols / 2)

    plot for [j=1:nlines] df using (column(2*j-1)):(column(2*j)) with lines title columnheader(2*j)
}

unset multiplot