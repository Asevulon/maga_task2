if (!exists("datafiles")) datafiles = "data.txt"
if (!exists("output_file")) output_file = "graph.png"
if (!exists("titles")) titles = "default_title"
if (!exists("width")) width = 800
if (!exists("height")) height = 600
if (!exists("xlabels")) xlabels = "X"
if (!exists("ylabels")) ylabels = "Y"
if (!exists("gui_mode")) gui_mode = 0

if (gui_mode) {
    set terminal qt size width, height
    unset mouse
} else {
    set terminal pngcairo size width, height font "Sans,12"
    set output output_file
}

N = words(datafiles)

set multiplot layout N,1 rowsfirst

do for [i = 1:N] {
    df   = word(datafiles, i)
    tit  = word(titles, i)
    xl   = word(xlabels, i)
    yl   = word(ylabels, i)

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

if (gui_mode)
    pause -1 "Нажмите Enter для выхода"