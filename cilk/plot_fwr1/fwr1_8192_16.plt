#set terminal png size 600, 600
#set output "fwr1_8192_16.png"
set datafile separator ","
set size square 1.0, 1.0
set title "Trial results for 'fwr1_8192_16'"
set xlabel "Worker Count" 0.0,0.0
set ylabel "Speedup" 0.0,0.0
set key left top box
set pointsize 1.8
plot [0:16] [0:16] x title "parallelism" lt 2, 13.329996 notitle lt 2, "fwr1_8192_16.csv" using 1:2 title "burdened speedup" with lines lt 1, "fwr1_8192_16.csv" using 1:4 with points pt 1 title "trials"
