#!/bin/bash

#echo "$1"

pid=`ps -ef | grep  rundb | grep -v grep | grep -v bash | awk '{print $2}'`
 
echo "$pid"
 
if [ -n "$pid" ]
then
        echo "kill -9 pid:$pid"
kill -9 $pid
fi

