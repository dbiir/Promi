#!/bin/bash
set -x

node_cnt=0
path=`pwd`"/ifconfig.txt"
while read line
do
    machine_array[node_cnt]=$line
    node_cnt=`expr $node_cnt + 1`
done < $path

node_cnt=$1

path=`pwd`"/netconfig.txt"

line_count=0
while read line
do
    delay_lines[line_count]=$line
    line_count=`expr $line_count + 1`
done < $path

node_id=0
for((i=0; i<${#delay_lines[@]}; i++)) 
do
    line=${delay_lines[i]}
    delay_array=(${line//,/ })  
    # ssh ${machine_array[node_id]} "sudo tc qdisc del root dev em1 2>/dev/null"
    # ssh ${machine_array[node_id]} "sudo tc qdisc add dev em1 root handle 1:0 htb"
    ssh ${machine_array[node_id]} "sudo tc qdisc del root dev eth0 2>/dev/null"
    ssh ${machine_array[node_id]} "sudo tc qdisc add dev eth0 root handle 1:0 htb"
    dst_node=0
    class_group=0
    for delay in ${delay_array[@]}
    do
        # echo $delay
        if [ $dst_node -ne $node_id ]; then
            class_group=`expr $class_group + 1`
            # ssh ${machine_array[node_id]} "sudo tc filter add dev em1 parent 1:0 prior 2 protocol ip u32 match ip dst ${machine_array[dst_node]} classid 1:$class_group"
            # ssh ${machine_array[node_id]} "sudo tc class add dev em1 parent 1:0 classid 1:$class_group htb rate 10Gbit"
            # ssh ${machine_array[node_id]} "sudo tc qdisc add dev em1 parent 1:$class_group handle `expr $class_group + 1`:0 netem delay ${delay}ms"
            ssh ${machine_array[node_id]} "sudo tc filter add dev eth0 parent 1:0 prior 2 protocol ip u32 match ip dst ${machine_array[dst_node]} classid 1:$class_group"
            ssh ${machine_array[node_id]} "sudo tc class add dev eth0 parent 1:0 classid 1:$class_group htb rate 10Gbit"
            ssh ${machine_array[node_id]} "sudo tc qdisc add dev eth0 parent 1:$class_group handle `expr $class_group + 1`:0 netem delay ${delay}ms"
            # # echo "ssh ${machine_array[node_id]} sudo tc filter add dev eth0 parent 1:0 prior 2 protocol ip u32 match ip dst ${machine_array[dst_node]} classid 1:$class_group"
            # echo "ssh ${machine_array[node_id]} sudo tc class add dev eth0 parent 1:0 classid 1:$class_group htb rate 10Gbit"
            # echo "ssh ${machine_array[node_id]} sudo tc qdisc add dev eth0 parent 1:$class_group handle `expr $class_group + 1`:0 netem delay ${delay}ms"
        fi
        dst_node=`expr $dst_node + 1`
    done 
    node_id=`expr $node_id + 1`
    # echo "$line"
done;
