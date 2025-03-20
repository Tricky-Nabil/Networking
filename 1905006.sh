#!/bin/bash
folder=scratch/ns3_2
datafile=$folder/data.txt
bottleneck_rate=50
packetloss_exp=6
tcp_part_2="ns3::TcpHighSpeed"

mkdir -p "$folder/NewRenoVSHighSpeed"
mkdir -p "$folder/NewRenoVSAdaptiveReno"
rm -f $datafile
touch $datafile

bottleneck_rate_array=("1" "2" "4" "8" "16" "32" "40" "50" "60" "70" "80" "90" "100" "125" "150" "175" "200" "225" "250" "275" "300")
packetloss_exp_array=("2" "2.5" "3" "3.5" "4" "4.5" "5" "5.5" "6")

# parameters - title, xlabel, ylabel, input file1, output file, xcolumn, y column1, y tick1, input file2, y column2, y tick2
plot_graph () {
gnuCommand="set terminal pngcairo size 1024,768 enhanced font 'Verdana,12';
set output '$5.png';
set title '$1' font 'Verdana,16';
set xlabel '$2' font 'Verdana,14';
set ylabel '$3' font 'Verdana,14';
plot '$4' using $6:$7 title '$8' with linespoints linestyle 1"


if [[ $# -gt 8 ]]
then
gnuCommand=$gnuCommand", '$9' using $6:${10} title '${11}' with
linespoints linestyle 2;"
else
gnuCommand=$gnuCommand";"
fi

if [[ $# -gt 11 ]]
then
gnuCommand=${gnuCommand//linespoints/lines}
fi

gnuCommand=$gnuCommand" exit;"

echo $gnuCommand | gnuplot
}


for item in "${bottleneck_rate_array[@]}"; do
    ./ns3 run "scratch/1905006.cc --tcp_part_2=$tcp_part_2 --bottleneck_rate=$item --packetloss_exp=$packetloss_exp" 
done

plot_graph "Throughput vs Bottleneck Rate" "Bottleneck Rate" "Throughput" "scratch/ns3_2/data.txt" "scratch/ns3_2/NewRenoVSHighSpeed/throughput_vs_bottleneckRate" 1 3 "TcpNewReno" "scratch/ns3_2/data.txt" 4 "TcpHighSpeed"
#plot_graph "Fairness Index vs Bottleneck Rate" "Bottleneck Rate" "Fairness Index" "scratch/ns3_2/data.txt" "scratch/ns3_2/NewRenoVSHighSpeed/fairness_index_vs_bottleneckRate" 1 5 "Fairness Index"

rm -f $datafile
touch $datafile

for item in "${packetloss_exp_array[@]}"; do
    ./ns3 run "scratch/1905006.cc --tcp_part_2=$tcp_part_2 --bottleneck_rate=$bottleneck_rate --packetloss_exp=$item" 
done

plot_graph "Throughput vs Packetloss Ratio" "Packetloss Ratio" "Throughput" "scratch/ns3_2/data.txt" "scratch/ns3_2/NewRenoVSHighSpeed/throughput_vs_packetloss_ratio" 2 3 "TcpNewReno" "scratch/ns3_2/data.txt" 4 "TcpHighSpeed"
#plot_graph "Fairness Index vs Packetloss Ratio" "Packetloss Ratio" "Fairness Index" "scratch/ns3_2/data.txt" "scratch/ns3_2/NewRenoVSHighSpeed/fairness_index_vs_packetloss_ratio" 2 5 "Fairness Index"

./ns3 run "scratch/1905006.cc --tcp_part_2=$tcp_part_2 --bottleneck_rate=$bottleneck_rate --packetloss_exp=$packetloss_exp"

plot_graph "Congestion Window vs Time" "Time" "Congestion Window" "scratch/ns3_2/flow1.cwnd" "scratch/ns3_2/NewRenoVSHighSpeed/Congestion_window_vs_time" 1 2 "TcpNewReno" "scratch/ns3_2/flow2.cwnd" 2 "TcpHighSpeed"



tcp_part_2="ns3::TcpAdaptiveReno"
rm -f $datafile
touch $datafile

for item in "${bottleneck_rate_array[@]}"; do
    ./ns3 run "scratch/1905006.cc --tcp_part_2=$tcp_part_2 --bottleneck_rate=$item --packetloss_exp=$packetloss_exp" 
done

plot_graph "Throughput vs Bottleneck Rate" "Bottleneck Rate" "Throughput" "scratch/ns3_2/data.txt" "scratch/ns3_2/NewRenoVSAdaptiveReno/throughput_vs_bottleneckRate" 1 3 "TcpNewReno" "scratch/ns3_2/data.txt" 4 "TcpAdaptiveReno"
#plot_graph "Fairness Index vs Bottleneck Rate" "Bottleneck Rate" "Fairness Index" "scratch/ns3_2/data.txt" "scratch/ns3_2/NewRenoVSAdaptiveReno/fairness_index_vs_bottleneckRate" 1 5 "Fairness Index"

rm -f $datafile
touch $datafile

for item in "${packetloss_exp_array[@]}"; do
    ./ns3 run "scratch/1905006.cc --tcp_part_2=$tcp_part_2 --bottleneck_rate=$bottleneck_rate --packetloss_exp=$item" 
done

plot_graph "Throughput vs Packetloss Ratio" "Packetloss Ratio" "Throughput" "scratch/ns3_2/data.txt" "scratch/ns3_2/NewRenoVSAdaptiveReno/throughput_vs_packetloss_ratio" 2 3 "TcpNewReno" "scratch/ns3_2/data.txt" 4 "TcpAdaptiveReno"
#plot_graph "Fairness Index vs Packetloss Ratio" "Packetloss Ratio" "Fairness Index" "scratch/ns3_2/data.txt" "scratch/ns3_2/NewRenoVSAdaptiveReno/fairness_index_vs_packetloss_ratio" 2 5 "Fairness Index"

./ns3 run "scratch/1905006.cc --tcp_part_2=$tcp_part_2 --bottleneck_rate=$bottleneck_rate --packetloss_exp=$packetloss_exp"

plot_graph "Congestion Window vs Time" "Time" "Congestion Window" "scratch/ns3_2/flow1.cwnd" "scratch/ns3_2/NewRenoVSAdaptiveReno/Congestion_window_vs_time" 1 2 "TcpNewReno" "scratch/ns3_2/flow2.cwnd" 2 "TcpAdaptiveReno"