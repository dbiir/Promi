Wooking+M
=======
Implementation of the paper “Wooking+M: Progressive and Efficient Live Migration in Distributed Database Systems”
Wooking+M is a testbed of an OLTP distributed database management system (DBMS).


## Dependencies
To ensure that the code works properly, the server needs to have the following dependencies:

    g++ >= 6.4.0
    Boost = 1.6.1
    jemalloc >= 5.2.1
    nanomsg >= 1.1.5
    libevent >= 1.2
    libibverbs

Build & Test
------------

To build the database.

    make clean
    make -j

Configuration
-------------

Wooking+M configurations can be changed in the config.h file. Please refer to README for the meaning of each configuration. Here we only list several most important ones.

    NODE_CNT          : Number of server nodes in the database
    THREAD_CNT        : Number of worker threads running per server
    WORKLOAD          : Supported workloads include YCSB and TPCC
    MAX_TXN_IN_FLIGHT  : Maximum number of active transactions at each server at a given time
    DONE_TIMER        : Amount of time to run experiment

Configurations can also be specified as command argument at runtime. Run the following command for a full list of program argument.

    ./rundb -h

Run
---

The Wooking+M can be run with

    ./rundb -nid[N]
    ./runcl -nid[M]

where N and M are the ID of a server and client, respectively

The `ifconfig.txt` file records the IP address that machine you want to test.
Here is an example of this file:

    10.77.110.148
    10.77.110.148

And run with

    ./rundb -nid0
    ./rundb -nid1
