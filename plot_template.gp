if (!exists("datafile")) datafile = "data.txt"
if (!exists("output_file")) output_file = "graph.png"
if (!exists("custom_title")) custom_title = "default_title"
if (!exists("width")) width = 800
if (!exists("height")) height = 600
if (!exists("x_label")) x_label = "X"
if (!exists("y_label")) y_label = "Y"
if (!exists("gui_mode")) gui_mode = 0

if (gui_mode) {
    set terminal qt size width, height
} else {
    set terminal pngcairo size width, height font "Sans,12"
    set output output_file
}

set title custom_title
set xlabel x_label
set ylabel y_label
set grid
set key outside top left
set offsets 0, 0, 0.1, 0.1

stats datafile nooutput
ncols = STATS_columns
nlines = int(ncols / 2)

plot for [i=1:nlines] datafile using (column(2*i-1)):(column(2*i)) with lines title columnheader(2*i)

if (gui_mode)
    pause -1 "Нажмите Enter для выхода"