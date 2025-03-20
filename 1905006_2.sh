#!/bin/bash

mkdir -p scratch/runs

touch throughput.dat
touch deliveryratio.dat

rm throughput.dat
rm deliveryratio.dat

declare -a node_num=("20" "40" "60" "80" "100")
declare -a flow_num=("10" "20" "30" "40" "50")
declare -a packet_num=("100" "200" "300" "400" "500")
declare -a speed=("5" "10" "15" "20" "25")

for i in ${node_num[@]}
do
    ./ns3 run "1905006_2 --node_num=${i} --graph=0"
    echo node_num $i
done

cp throughput.dat scratch/runs/throughput_node.dat
cp deliveryratio.dat scratch/runs/deliveryratio_node.dat

rm throughput.dat
rm deliveryratio.dat

for i in ${flow_num[@]}
do 
    ./ns3 run "1905006_2 --flow_num=${i} --graph=1"
    echo flow_num $i
done

cp throughput.dat scratch/runs/throughput_flow.dat
cp deliveryratio.dat scratch/runs/deliveryratio_flow.dat

rm throughput.dat
rm deliveryratio.dat

for i in ${packet_num[@]}
do  
    ./ns3 run "1905006_2 --packet_num=${i} --graph=2"
    echo packet_num $i
done

cp throughput.dat scratch/runs/throughput_packet.dat
cp deliveryratio.dat scratch/runs/deliveryratio_packet.dat

rm throughput.dat
rm deliveryratio.dat

for i in ${speed[@]}
do
    ./ns3 run "1905006_2 --speed=${i} --graph=3"
    echo speed $i
done

cp throughput.dat scratch/runs/throughput_speed.dat
cp deliveryratio.dat scratch/runs/deliveryratio_speed.dat

gnuplot -persist <<-EOFMarker
    set terminal png size 640,480
    set output "scratch/runs/throughput_vs_node.png"
    set xlabel "nodes"
    set ylabel "throughput"
    plot "scratch/runs/throughput_node.dat" using 1:2 title 'Throughput vs nodes' with linespoints

    set output "scratch/runs/delivery_ratio_vs_node.png"
    set xlabel "nodes"
    set ylabel "delivery ratio"
    plot "scratch/runs/deliveryratio_node.dat" using 1:2 title 'Delivery ratio vs nodes' with linespoints

    set output "scratch/runs/throughput_vs_flow.png"
    set xlabel "flows"
    set ylabel "throughput"
    plot "scratch/runs/throughput_flow.dat" using 1:2 title 'Throughput vs flows' with linespoints

    set output "scratch/runs/delivery_ratio_vs_flow.png"
    set xlabel "flows"
    set ylabel "delivery ratio"
    plot "scratch/runs/deliveryratio_flow.dat" using 1:2 title 'Delivery ratio vs flows' with linespoints

    set output "scratch/runs/throughput_vs_packet.png"
    set xlabel "packet"
    set ylabel "throughput"
    plot "scratch/runs/throughput_packet.dat" using 1:2 title 'Throughput vs packets' with linespoints

    set output "scratch/runs/delivery_ratio_vs_packet.png"
    set xlabel "packet"
    set ylabel "delivery ratio"
    plot "scratch/runs/deliveryratio_packet.dat" using 1:2 title 'Delivery ratio vs packets' with linespoints

    set output "scratch/runs/throughput_vs_speed.png"
    set xlabel "speed"
    set ylabel "throughput"
    plot "scratch/runs/throughput_speed.dat" using 1:2 title 'Throughput vs speed' with linespoints

    set output "scratch/runs/delivery_ratio_vs_speed.png"
    set xlabel "speed"
    set ylabel "delivery ratio"
    plot "scratch/runs/deliveryratio_speed.dat" using 1:2 title 'Delivery ratio vs speed' with linespoints
EOFMarker

