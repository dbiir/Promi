#!/bin/bash
set -x

node_cnt=0
path=`pwd`"/ifconfig.txt"
while read line
do
    machine_array[node_cnt]=$line
    node_cnt=`expr $node_cnt + 1`
done < $path

node_cnt=`expr $1 - 1`

for node_id in $(seq 0 $node_cnt)
do
    ssh ${machine_array[node_id]} "sudo tc qdisc del root dev em1 2>/dev/null"
done