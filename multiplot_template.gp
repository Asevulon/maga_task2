# Список файлов и настроек
if (!exists("datafiles")) datafiles = "data.txt"
if (!exists("output_file")) output_file = "graph.png"
if (!exists("titles")) titles = "default_title"
if (!exists("width")) width = 800
if (!exists("height")) height = 600
if (!exists("xlabels")) xlabels = "X"
if (!exists("ylabels")) ylabels = "Y"

set terminal pngcairo size width, height font "Sans,12"
set output output_file

# Разбираем строки в массивы (Gnuplot ≥5.0)
N = words(datafiles)

# Включаем multiplot: 2 строки, 1 столбец
set multiplot layout N,1 rowsfirst

do for [i = 1:N] {
    df   = word(datafiles, i)
    tit  = word(titles, i)
    xl   = word(xlabels, i)
    yl   = word(ylabels, i)

    # Настройки для текущего подграфика
    set title tit
    set xlabel xl
    set ylabel yl
    set grid
    set key outside top left
    set offsets 0, 0, 0.1, 0.1

    # Определяем количество линий (как у вас)
    stats df nooutput
    ncols = STATS_columns
    nlines = int(ncols / 2)

    # Рисуем
    plot for [j=1:nlines] df using (column(2*j-1)):(column(2*j)) with lines title columnheader(2*j)
}

unset multiplot