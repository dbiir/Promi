import itertools
from paper_plots import *
# Experiments to run and analyze
# Go to end of file to fill in experiments
SHORTNAMES = {
    "CLIENT_NODE_CNT" : "CN",
    "CLIENT_THREAD_CNT" : "CT",
    "CLIENT_REM_THREAD_CNT" : "CRT",
    "CLIENT_SEND_THREAD_CNT" : "CST",
    "NODE_CNT" : "N",
    "THREAD_CNT" : "T",
    "REM_THREAD_CNT" : "RT",
    "SEND_THREAD_CNT" : "ST",
    "CC_ALG" : "",
    "WORKLOAD" : "",
    "MAX_TXN_PER_PART" : "TXNS",
    "MAX_TXN_IN_FLIGHT" : "TIF",
    "PART_PER_TXN" : "PPT",
    "TUP_READ_PERC" : "TRD",
    "TUP_WRITE_PERC" : "TWR",
    "TXN_READ_PERC" : "RD",
    "TXN_WRITE_PERC" : "WR",
    "ZIPF_THETA" : "SKEW",
    "MSG_TIME_LIMIT" : "BT",
    "MSG_SIZE_MAX" : "BS",
    "DATA_PERC":"D",
    "ACCESS_PERC":"A",
    "PERC_PAYMENT":"PP",
    "MPR":"MPR",
    "REQ_PER_QUERY": "RPQ",
    "MODE":"",
    "PRIORITY":"",
    "ABORT_PENALTY":"PENALTY",
    "STRICT_PPT":"SPPT",
    "NETWORK_DELAY":"NDLY",
    "NETWORK_DELAY_TEST":"NDT",
    "REPLICA_CNT":"RN",
    "SYNTH_TABLE_SIZE":"TBL",
    "ISOLATION_LEVEL":"LVL",
    "YCSB_ABORT_MODE":"ABRTMODE",
    "NUM_WH":"WH",
}

fmt_title=["NODE_CNT","CC_ALG","ACCESS_PERC","TXN_WRITE_PERC","PERC_PAYMENT","MPR","MODE","MAX_TXN_IN_FLIGHT","SEND_THREAD_CNT","REM_THREAD_CNT","THREAD_CNT","TXN_WRITE_PERC","TUP_WRITE_PERC","ZIPF_THETA","NUM_WH"]

##############################
# PLOTS
##############################
#dta_target_algos=['DLI_BASE','DLI_MVCC_OCC','DLI_MVCC_BASE','DLI_OCC','MAAT']#['DLI_MVCC_OCC','DLI_DTA','TIMESTAMP','WAIT_DIE']#['NO_WAIT','WAIT_DIE','MVCC','MAAT','CALVIN','TIMESTAMP','OCC','DLI_MVCC_OCC','DLI_OCC','DLI_BASE','DLI_MVCC_BASE','DLI_DTA']
#dta_target_algos=['NO_WAIT', 'MVCC', 'CALVIN', 'MAAT']
#dta_target_algos=['DLI_DTA3']
#dta_target_algos=['NO_WAIT','WAIT_DIE','MVCC','MAAT','CALVIN','TIMESTAMP','OCC','DLI_MVCC_OCC','DLI_BASE','DLI_MVCC_BASE']
dta_target_algos=['TIMESTAMP']
# tpcc load
#tpcc_loads = ['50', '100', '200', '500', '1000', '2000', '5000']
tpcc_loads = ['50', '100', '200', '500', '1000', '2000', '5000']
# ycsb load
ycsb_loads = ['50', '100', '200', '500', '1000', '2000', '5000']

def pps_scaling():
    wl = 'PPS'
    nnodes = [1,2,4,8,16,32,64]
    nalgos=['NO_WAIT','WAIT_DIE','MVCC','MAAT','CALVIN','TIMESTAMP','WOOKONG']
    load = [10000]
    fmt = ["WORKLOAD","NODE_CNT","CC_ALG","MAX_TXN_IN_FLIGHT"]
    exp = [[wl,n,cc,tif] for tif,n,cc in itertools.product(load,nnodes,nalgos)]
    return fmt,exp


def ycsb_scaling():
    wl = 'YCSB'
    #nnodes = [1,2,4,8,16,32,64]
    #nnodes = [1,2,4,8,16,32]
    nnodes = [2]
	  # algos=['WOOKONG','WAIT_DIE','MVCC','MAAT','TIMESTAMP','OCC']
    # algos=['MVCC','MAAT','TIMESTAMP','WOOKONG','OCC']
    # algos=['MAAT','MVCC','TIMESTAMP','OCC','DLI_DTA3','DLI_OCC']
    algos=['SSI']
    base_table_size=65536
    #base_table_size=2097152*8
    txn_write_perc = [0.0]
    tup_write_perc = [0.0]
    load = [10000]
    tcnt = [4]
    ctcnt = [4]
    scnt = [2]
    rcnt = [2]
    skew = [0.0]
    #skew = [0.0,0.5,0.9]
    fmt = ["WORKLOAD","NODE_CNT","CC_ALG","SYNTH_TABLE_SIZE","TUP_WRITE_PERC","TXN_WRITE_PERC","MAX_TXN_IN_FLIGHT","ZIPF_THETA","THREAD_CNT","CLIENT_THREAD_CNT","SEND_THREAD_CNT","REM_THREAD_CNT","CLIENT_SEND_THREAD_CNT","CLIENT_REM_THREAD_CNT"]
    exp = [[wl,n,algo,base_table_size*n,tup_wr_perc,txn_wr_perc,ld,sk,thr,cthr,sthr,rthr,sthr,rthr] for thr,cthr,sthr,rthr,txn_wr_perc,tup_wr_perc,sk,ld,n,algo in itertools.product(tcnt,ctcnt,scnt,rcnt,txn_write_perc,tup_write_perc,skew,load,nnodes,algos)]
    #txn_write_perc = [0.0]
    #skew = [0.0]
    #exp = exp + [[wl,n,algo,base_table_size*n,tup_wr_perc,txn_wr_perc,ld,sk,thr] for thr,txn_wr_perc,tup_wr_perc,sk,ld,n,algo in itertools.product(tcnt,txn_write_perc,tup_write_perc,skew,load,nnodes,algos)]
    return fmt,exp

def ycsb_scaling1():
    wl = 'YCSB'
    #nnodes = [1,2,4,8,16,32,64]
    nnodes = [1,2]
    algos=['MAAT','MVCC','TIMESTAMP','OCC','DLI_DTA3','DLI_OCC']
    base_table_size=1048576*8
    txn_write_perc = [0.5]
    tup_write_perc = [0.5]
    load = [10000]
    tcnt = [4]
    ctcnt = [4]
    skew = [0.5]
    fmt = ["WORKLOAD","NODE_CNT","CC_ALG","SYNTH_TABLE_SIZE","TUP_WRITE_PERC","TXN_WRITE_PERC","MAX_TXN_IN_FLIGHT","ZIPF_THETA","THREAD_CNT","CLIENT_THREAD_CNT"]
    exp = [[wl,n,algo,base_table_size*n,tup_wr_perc,txn_wr_perc,ld,sk,thr,cthr] for thr,cthr,txn_wr_perc,tup_wr_perc,sk,ld,n,algo in itertools.product(tcnt,ctcnt,txn_write_perc,tup_write_perc,skew,load,nnodes,algos)]
    return fmt,exp

def ecwc():
    wl = 'YCSB'
    nnodes = [2]
    algos=['NO_WAIT','WAIT_DIE','MVCC','CALVIN','TIMESTAMP','MAAT','WOOKONG']
    base_table_size=2097152*8
    txn_write_perc = [0.5]
    tup_write_perc = [0.5]
    load = [10000]
    tcnt = [4]
    skew = [0.6]
    fmt = ["WORKLOAD","NODE_CNT","CC_ALG","SYNTH_TABLE_SIZE","TUP_WRITE_PERC","TXN_WRITE_PERC","MAX_TXN_IN_FLIGHT","ZIPF_THETA","THREAD_CNT"]
    exp = [[wl,n,algo,base_table_size*n,tup_wr_perc,txn_wr_perc,ld,sk,thr] for thr,txn_wr_perc,tup_wr_perc,sk,ld,n,algo in itertools.product(tcnt,txn_write_perc,tup_write_perc,skew,load,nnodes,algos)]
    return fmt,exp


def ycsb_scaling_abort():
    wl = 'YCSB'
    nnodes = [1,2,4,8,16,32,64]
    algos=['NO_WAIT','WAIT_DIE','MVCC','MAAT','CALVIN','TIMESTAMP','WOOKONG']
    base_table_size=2097152*8
    txn_write_perc = [0.5]
    tup_write_perc = [0.5]
    load = [10000,12000]
    load = [10000]
    tcnt = [4]
    skew = [0.6,0.7]
    fmt = ["WORKLOAD","NODE_CNT","CC_ALG","SYNTH_TABLE_SIZE","TUP_WRITE_PERC","TXN_WRITE_PERC","MAX_TXN_IN_FLIGHT","ZIPF_THETA","THREAD_CNT","YCSB_ABORT_MODE"]
    exp = [[wl,n,algo,base_table_size*n,tup_wr_perc,txn_wr_perc,ld,sk,thr,'true'] for thr,txn_wr_perc,tup_wr_perc,sk,ld,n,algo in itertools.product(tcnt,txn_write_perc,tup_write_perc,skew,load,nnodes,algos)]
    return fmt,exp


def ycsb_skew():
    wl = 'YCSB'
    nnodes = [1]
    #algos=['WOOKONG','WAIT_DIE','MVCC','MAAT','TIMESTAMP','OCC']
    algos=dta_target_algos
    base_table_size=2097152*8
    txn_write_perc = [0.5]
    tup_write_perc = [0.5]
    load = [10000]
    tcnt = [4]
    #skew = [0.0,0.25,0.5,0.55,0.6,0.65,0.7,0.75,0.8,0.9]
    skew = [0.0,0.6,0.9]
    fmt = ["WORKLOAD","NODE_CNT","CC_ALG","SYNTH_TABLE_SIZE","TUP_WRITE_PERC","TXN_WRITE_PERC","MAX_TXN_IN_FLIGHT","ZIPF_THETA","THREAD_CNT"]
    exp = [[wl,n,algo,base_table_size*n,tup_wr_perc,txn_wr_perc,ld,sk,thr] for thr,txn_wr_perc,tup_wr_perc,ld,n,sk,algo in itertools.product(tcnt,txn_write_perc,tup_write_perc,load,nnodes,skew,algos)]
    return fmt,exp

def ycsb_skew1():
    wl = 'YCSB'
    nnodes = [2]
    algos=dta_target_algos
    base_table_size=2097152*8
    txn_write_perc = [0.5]
    tup_write_perc = [0.5]
    load = [10000]
    tcnt = [4]
   # skew = [0.0,0.25,0.5,0.55,0.6,0.65,0.7,0.75,0.8,0.9]
    skew = [0.0,0.6,0.9]
    fmt = ["WORKLOAD","NODE_CNT","CC_ALG","SYNTH_TABLE_SIZE","TUP_WRITE_PERC","TXN_WRITE_PERC","MAX_TXN_IN_FLIGHT","ZIPF_THETA","THREAD_CNT"]
    exp = [[wl,n,algo,base_table_size*n,tup_wr_perc,txn_wr_perc,ld,sk,thr] for thr,txn_wr_perc,tup_wr_perc,ld,n,sk,algo in itertools.product(tcnt,txn_write_perc,tup_write_perc,load,nnodes,skew,algos)]
    return fmt,exp

def ycsb_stress1():
    wl = 'YCSB'
    nnodes = [1]
    #algos=['WOOKONG','WAIT_DIE','MVCC','MAAT','TIMESTAMP','OCC']
    algos=dta_target_algos
    base_table_size=2097152*8
    txn_write_perc = [0.5]
    tup_write_perc = [0.5]
    load = ycsb_loads
    #load = ycsb_loads
    tcnt = [4]
    #skew = [0.0,0.25,0.5,0.55,0.6,0.65,0.7,0.75,0.8,0.9]
    skew = [0.0]
    fmt = ["WORKLOAD","NODE_CNT","CC_ALG","SYNTH_TABLE_SIZE","TUP_WRITE_PERC","TXN_WRITE_PERC","MAX_TXN_IN_FLIGHT","ZIPF_THETA","THREAD_CNT"]
    exp = [[wl,n,algo,base_table_size*n,tup_wr_perc,txn_wr_perc,ld,sk,thr] for thr,txn_wr_perc,tup_wr_perc,ld,n,sk,algo in itertools.product(tcnt,txn_write_perc,tup_write_perc,load,nnodes,skew,algos)]
    return fmt,exp

def ycsb_stress2():
    wl = 'YCSB'
    nnodes = [1]
    #algos=['WOOKONG','WAIT_DIE','MVCC','MAAT','TIMESTAMP','OCC']
    algos=dta_target_algos
    base_table_size=2097152*8
    txn_write_perc = [0.5]
    tup_write_perc = [0.5]
    load = ycsb_loads
    #load = ycsb_loads
    tcnt = [4]
    #skew = [0.0,0.25,0.5,0.55,0.6,0.65,0.7,0.75,0.8,0.9]
    skew = [0.25]
    fmt = ["WORKLOAD","NODE_CNT","CC_ALG","SYNTH_TABLE_SIZE","TUP_WRITE_PERC","TXN_WRITE_PERC","MAX_TXN_IN_FLIGHT","ZIPF_THETA","THREAD_CNT"]
    exp = [[wl,n,algo,base_table_size*n,tup_wr_perc,txn_wr_perc,ld,sk,thr] for thr,txn_wr_perc,tup_wr_perc,ld,n,sk,algo in itertools.product(tcnt,txn_write_perc,tup_write_perc,load,nnodes,skew,algos)]
    return fmt,exp

def ycsb_stress3():
    wl = 'YCSB'
    nnodes = [1]
    #algos=['WOOKONG','WAIT_DIE','MVCC','MAAT','TIMESTAMP','OCC']
    algos=dta_target_algos
    base_table_size=2097152*8
    txn_write_perc = [0.5]
    tup_write_perc = [0.5]
    load = ycsb_loads
    #load = ycsb_loads
    tcnt = [4]
    #skew = [0.0,0.25,0.5,0.55,0.6,0.65,0.7,0.75,0.8,0.9]
    skew = [0.55]
    fmt = ["WORKLOAD","NODE_CNT","CC_ALG","SYNTH_TABLE_SIZE","TUP_WRITE_PERC","TXN_WRITE_PERC","MAX_TXN_IN_FLIGHT","ZIPF_THETA","THREAD_CNT"]
    exp = [[wl,n,algo,base_table_size*n,tup_wr_perc,txn_wr_perc,ld,sk,thr] for thr,txn_wr_perc,tup_wr_perc,ld,n,sk,algo in itertools.product(tcnt,txn_write_perc,tup_write_perc,load,nnodes,skew,algos)]
    return fmt,exp

def ycsb_stress4():
    wl = 'YCSB'
    nnodes = [1]
    #algos=['WOOKONG','WAIT_DIE','MVCC','MAAT','TIMESTAMP','OCC']
    algos=dta_target_algos
    base_table_size=2097152*8
    txn_write_perc = [0.5]
    tup_write_perc = [0.5]
    load = ycsb_loads
    #load = ycsb_loads
    tcnt = [4]
    #skew = [0.0,0.25,0.5,0.55,0.6,0.65,0.7,0.75,0.8,0.9]
    skew = [0.65]
    fmt = ["WORKLOAD","NODE_CNT","CC_ALG","SYNTH_TABLE_SIZE","TUP_WRITE_PERC","TXN_WRITE_PERC","MAX_TXN_IN_FLIGHT","ZIPF_THETA","THREAD_CNT"]
    exp = [[wl,n,algo,base_table_size*n,tup_wr_perc,txn_wr_perc,ld,sk,thr] for thr,txn_wr_perc,tup_wr_perc,ld,n,sk,algo in itertools.product(tcnt,txn_write_perc,tup_write_perc,load,nnodes,skew,algos)]
    return fmt,exp

def ycsb_stress5():
    wl = 'YCSB'
    nnodes = [1]
    #algos=['WOOKONG','WAIT_DIE','MVCC','MAAT','TIMESTAMP','OCC']
    algos=dta_target_algos
    base_table_size=2097152*8
    txn_write_perc = [0.5]
    tup_write_perc = [0.5]
    load = ycsb_loads
    #load = ycsb_loads
    tcnt = [4]
    #skew = [0.0,0.25,0.5,0.55,0.6,0.65,0.7,0.75,0.8,0.9]
    skew = [0.75]
    fmt = ["WORKLOAD","NODE_CNT","CC_ALG","SYNTH_TABLE_SIZE","TUP_WRITE_PERC","TXN_WRITE_PERC","MAX_TXN_IN_FLIGHT","ZIPF_THETA","THREAD_CNT"]
    exp = [[wl,n,algo,base_table_size*n,tup_wr_perc,txn_wr_perc,ld,sk,thr] for thr,txn_wr_perc,tup_wr_perc,ld,n,sk,algo in itertools.product(tcnt,txn_write_perc,tup_write_perc,load,nnodes,skew,algos)]
    return fmt,exp

def ycsb_stress6():
    wl = 'YCSB'
    nnodes = [1]
    #algos=['WOOKONG','WAIT_DIE','MVCC','MAAT','TIMESTAMP','OCC']
    algos=dta_target_algos
    base_table_size=2097152*8
    txn_write_perc = [0.5]
    tup_write_perc = [0.5]
    load = ycsb_loads
    #load = ycsb_loads
    tcnt = [4]
    #skew = [0.0,0.25,0.5,0.55,0.6,0.65,0.7,0.75,0.8,0.9]
    skew = [0.9]
    fmt = ["WORKLOAD","NODE_CNT","CC_ALG","SYNTH_TABLE_SIZE","TUP_WRITE_PERC","TXN_WRITE_PERC","MAX_TXN_IN_FLIGHT","ZIPF_THETA","THREAD_CNT"]
    exp = [[wl,n,algo,base_table_size*n,tup_wr_perc,txn_wr_perc,ld,sk,thr] for thr,txn_wr_perc,tup_wr_perc,ld,n,sk,algo in itertools.product(tcnt,txn_write_perc,tup_write_perc,load,nnodes,skew,algos)]
    return fmt,exp

def ycsb_stress7():
    wl = 'YCSB'
    nnodes = [1]
    #algos=['WOOKONG','WAIT_DIE','MVCC','MAAT','TIMESTAMP','OCC']
    algos=dta_target_algos
    base_table_size=2097152*8
    txn_write_perc = [0.0]
    tup_write_perc = [0.5]
    load = ycsb_loads
    #load = ycsb_loads
    tcnt = [4]
    #skew = [0.0,0.25,0.5,0.55,0.6,0.65,0.7,0.75,0.8,0.9]
    skew = [0.6]
    fmt = ["WORKLOAD","NODE_CNT","CC_ALG","SYNTH_TABLE_SIZE","TUP_WRITE_PERC","TXN_WRITE_PERC","MAX_TXN_IN_FLIGHT","ZIPF_THETA","THREAD_CNT"]
    exp = [[wl,n,algo,base_table_size*n,tup_wr_perc,txn_wr_perc,ld,sk,thr] for thr,txn_wr_perc,tup_wr_perc,ld,n,sk,algo in itertools.product(tcnt,txn_write_perc,tup_write_perc,load,nnodes,skew,algos)]
    return fmt,exp

def ycsb_stress8():
    wl = 'YCSB'
    nnodes = [1]
    #algos=['WOOKONG','WAIT_DIE','MVCC','MAAT','TIMESTAMP','OCC']
    algos=dta_target_algos
    base_table_size=2097152*8
    txn_write_perc = [0.2]
    tup_write_perc = [0.5]
    load = ycsb_loads
    #load = ycsb_loads
    tcnt = [4]
    #skew = [0.0,0.25,0.5,0.55,0.6,0.65,0.7,0.75,0.8,0.9]
    skew = [0.6]
    fmt = ["WORKLOAD","NODE_CNT","CC_ALG","SYNTH_TABLE_SIZE","TUP_WRITE_PERC","TXN_WRITE_PERC","MAX_TXN_IN_FLIGHT","ZIPF_THETA","THREAD_CNT"]
    exp = [[wl,n,algo,base_table_size*n,tup_wr_perc,txn_wr_perc,ld,sk,thr] for thr,txn_wr_perc,tup_wr_perc,ld,n,sk,algo in itertools.product(tcnt,txn_write_perc,tup_write_perc,load,nnodes,skew,algos)]
    return fmt,exp

def ycsb_stress9():
    wl = 'YCSB'
    nnodes = [1]
    #algos=['WOOKONG','WAIT_DIE','MVCC','MAAT','TIMESTAMP','OCC']
    algos=dta_target_algos
    base_table_size=2097152*8
    txn_write_perc = [0.4]
    tup_write_perc = [0.5]
    load = ycsb_loads
    #load = ycsb_loads
    tcnt = [4]
    #skew = [0.0,0.25,0.5,0.55,0.6,0.65,0.7,0.75,0.8,0.9]
    skew = [0.6]
    fmt = ["WORKLOAD","NODE_CNT","CC_ALG","SYNTH_TABLE_SIZE","TUP_WRITE_PERC","TXN_WRITE_PERC","MAX_TXN_IN_FLIGHT","ZIPF_THETA","THREAD_CNT"]
    exp = [[wl,n,algo,base_table_size*n,tup_wr_perc,txn_wr_perc,ld,sk,thr] for thr,txn_wr_perc,tup_wr_perc,ld,n,sk,algo in itertools.product(tcnt,txn_write_perc,tup_write_perc,load,nnodes,skew,algos)]
    return fmt,exp

def ycsb_stress10():
    wl = 'YCSB'
    nnodes = [1]
    #algos=['WOOKONG','WAIT_DIE','MVCC','MAAT','TIMESTAMP','OCC']
    algos=dta_target_algos
    base_table_size=2097152*8
    txn_write_perc = [0.6]
    tup_write_perc = [0.5]
    load = ycsb_loads
    #load = ycsb_loads
    tcnt = [4]
    #skew = [0.0,0.25,0.5,0.55,0.6,0.65,0.7,0.75,0.8,0.9]
    skew = [0.6]
    fmt = ["WORKLOAD","NODE_CNT","CC_ALG","SYNTH_TABLE_SIZE","TUP_WRITE_PERC","TXN_WRITE_PERC","MAX_TXN_IN_FLIGHT","ZIPF_THETA","THREAD_CNT"]
    exp = [[wl,n,algo,base_table_size*n,tup_wr_perc,txn_wr_perc,ld,sk,thr] for thr,txn_wr_perc,tup_wr_perc,ld,n,sk,algo in itertools.product(tcnt,txn_write_perc,tup_write_perc,load,nnodes,skew,algos)]
    return fmt,exp

def ycsb_stress11():
    wl = 'YCSB'
    nnodes = [1]
    #algos=['WOOKONG','WAIT_DIE','MVCC','MAAT','TIMESTAMP','OCC']
    algos=dta_target_algos
    base_table_size=2097152*8
    txn_write_perc = [0.8]
    tup_write_perc = [0.5]
    load = ycsb_loads
    #load = ycsb_loads
    tcnt = [4]
    #skew = [0.0,0.25,0.5,0.55,0.6,0.65,0.7,0.75,0.8,0.9]
    skew = [0.6]
    fmt = ["WORKLOAD","NODE_CNT","CC_ALG","SYNTH_TABLE_SIZE","TUP_WRITE_PERC","TXN_WRITE_PERC","MAX_TXN_IN_FLIGHT","ZIPF_THETA","THREAD_CNT"]
    exp = [[wl,n,algo,base_table_size*n,tup_wr_perc,txn_wr_perc,ld,sk,thr] for thr,txn_wr_perc,tup_wr_perc,ld,n,sk,algo in itertools.product(tcnt,txn_write_perc,tup_write_perc,load,nnodes,skew,algos)]
    return fmt,exp

def ycsb_stress12():
    wl = 'YCSB'
    nnodes = [1]
    #algos=['WOOKONG','WAIT_DIE','MVCC','MAAT','TIMESTAMP','OCC']
    algos=dta_target_algos
    base_table_size=2097152*8
    txn_write_perc = [1.0]
    tup_write_perc = [0.5]
    load = ycsb_loads
    #load = ycsb_loads
    tcnt = [4]
    #skew = [0.0,0.25,0.5,0.55,0.6,0.65,0.7,0.75,0.8,0.9]
    skew = [0.6]
    fmt = ["WORKLOAD","NODE_CNT","CC_ALG","SYNTH_TABLE_SIZE","TUP_WRITE_PERC","TXN_WRITE_PERC","MAX_TXN_IN_FLIGHT","ZIPF_THETA","THREAD_CNT"]
    exp = [[wl,n,algo,base_table_size*n,tup_wr_perc,txn_wr_perc,ld,sk,thr] for thr,txn_wr_perc,tup_wr_perc,ld,n,sk,algo in itertools.product(tcnt,txn_write_perc,tup_write_perc,load,nnodes,skew,algos)]
    return fmt,exp

def ycsb_writes():
    wl = 'YCSB'
    nnodes = [1]
    # algos=['WOOKONG','WAIT_DIE','MVCC','MAAT','TIMESTAMP','OCC']
    algos=dta_target_algos
    base_table_size=2097152*8
    txn_write_perc = [0.0,0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9,1.0]
    tup_write_perc = [0.5]
    load = [20000]
    tcnt = [4]
    skew = [0.6]
    fmt = ["WORKLOAD","NODE_CNT","CC_ALG","SYNTH_TABLE_SIZE","TUP_WRITE_PERC","TXN_WRITE_PERC","MAX_TXN_IN_FLIGHT","ZIPF_THETA","THREAD_CNT"]
    exp = [[wl,n,algo,base_table_size*n,tup_wr_perc,txn_wr_perc,ld,sk,thr] for thr,txn_wr_perc,tup_wr_perc,ld,n,sk,algo in itertools.product(tcnt,txn_write_perc,tup_write_perc,load,nnodes,skew,algos)]
    return fmt,exp



def isolation_levels():
    wl = 'YCSB'
    nnodes = [1,2,4,8,16,32,64]
    algos=['NO_WAIT']
    levels=["READ_UNCOMMITTED","READ_COMMITTED","SERIALIZABLE","NOLOCK"]
    base_table_size=2097152*8
    load = [10000]
    txn_write_perc = [0.5]
    tup_write_perc = [0.5]
    skew = [0.6,0.7]
    skew = [0.6]
    fmt = ["WORKLOAD","NODE_CNT","CC_ALG","SYNTH_TABLE_SIZE","TUP_WRITE_PERC","TXN_WRITE_PERC","ISOLATION_LEVEL","MAX_TXN_IN_FLIGHT","ZIPF_THETA"]
    exp = [[wl,n,algo,base_table_size*n,tup_wr_perc,txn_wr_perc,level,ld,sk] for txn_wr_perc,tup_wr_perc,algo,sk,ld,n,level in itertools.product(txn_write_perc,tup_write_perc,algos,skew,load,nnodes,levels)]
    return fmt,exp

def ycsb_partitions():
    wl = 'YCSB'
    nnodes = [16]
    algos=['NO_WAIT','WAIT_DIE','MVCC','MAAT','CALVIN','TIMESTAMP','WOOKONG']
    load = [10000,12000]
    load = [10000]
    nparts = [1,2,4,6,8,10,12,14,16]
    base_table_size=2097152*8
    txn_write_perc = [0.5]
    tup_write_perc = [0.5]
    tcnt = [4]
    skew = [0.6]
    rpq =  16
    fmt = ["WORKLOAD","REQ_PER_QUERY","PART_PER_TXN","NODE_CNT","CC_ALG","SYNTH_TABLE_SIZE","TUP_WRITE_PERC","TXN_WRITE_PERC","MAX_TXN_IN_FLIGHT","ZIPF_THETA","THREAD_CNT","STRICT_PPT"]
    exp = [[wl,rpq,p,n,algo,base_table_size*n,tup_wr_perc,txn_wr_perc,ld,sk,thr,1] for thr,txn_wr_perc,tup_wr_perc,algo,sk,ld,n,p in itertools.product(tcnt,txn_write_perc,tup_write_perc,algos,skew,load,nnodes,nparts)]
    return fmt,exp

def ycsb_partitions_distr():
    wl = 'YCSB'
    nnodes = [16]
    algos=['NO_WAIT','WAIT_DIE','MVCC','MAAT','CALVIN','TIMESTAMP','WOOKONG']
    load = [10000]
    nparts = [2,4,6,8,10,12,14,16]
    base_table_size=2097152*8
    txn_write_perc = [0.5]
    tup_write_perc = [0.5]
    tcnt = [4]
    skew = [0.6]
    rpq =  16
    fmt = ["WORKLOAD","REQ_PER_QUERY","PART_PER_TXN","NODE_CNT","CC_ALG","SYNTH_TABLE_SIZE","TUP_WRITE_PERC","TXN_WRITE_PERC","MAX_TXN_IN_FLIGHT","ZIPF_THETA","THREAD_CNT","STRICT_PPT"]
    exp = [[wl,rpq,p,n,algo,base_table_size*n,tup_wr_perc,txn_wr_perc,ld,sk,thr,1] for thr,txn_wr_perc,tup_wr_perc,algo,sk,ld,n,p in itertools.product(tcnt,txn_write_perc,tup_write_perc,algos,skew,load,nnodes,nparts)]
    return fmt,exp

def tpcc_scaling():
    wl = 'TPCC'
    nnodes = [1]
    # nalgos=['NO_WAIT','WAIT_DIE','MAAT','MVCC','TIMESTAMP','CALVIN','WOOKONG']
    nalgos=['NO_WAIT','WAIT_DIE','MAAT','MVCC','TIMESTAMP','OCC','CALVIN','WOOKONG','TICTOC','DLI_DTA','DLI_DTA1','DLI_DTA2','DLI_DTA3','DLI_MVCC_OCC','DLI_MVCC']
    # nalgos=['WOOKONG']
    # nalgos=['NO_WAIT']
    npercpay=[0.0]
    # npercpay=[0.0]
    wh=128
    # wh=64
    load = [10000,20000,30000,40000,50000]
    tcnt = [100]
    ctcnt = [100]
    fmt = ["WORKLOAD","NODE_CNT","CC_ALG","PERC_PAYMENT","NUM_WH","MAX_TXN_IN_FLIGHT","THREAD_CNT","CLIENT_THREAD_CNT"]
    exp = [[wl,n,cc,pp,wh*n,tif,thr,cthr] for thr,cthr,tif,pp,n,cc in itertools.product(tcnt,ctcnt,load,npercpay,nnodes,nalgos)]

    # wh=4
    # exp = exp+[[wl,n,cc,pp,wh*n,tif] for tif,pp,n,cc in itertools.product(load,npercpay,nnodes,nalgos)]
    return fmt,exp

def tpcc_cstress():
    wl = 'TPCC'
    nnodes = [1]
    # nalgos=['NO_WAIT','WAIT_DIE','MAAT','MVCC','TIMESTAMP','CALVIN','WOOKONG']
    # nalgos=['NO_WAIT','WAIT_DIE','MAAT','MVCC','TIMESTAMP','OCC','CALVIN','WOOKONG','TICTOC','DLI_DTA','DLI_DTA1','DLI_DTA2','DLI_DTA3','DLI_MVCC_OCC','DLI_MVCC']
    # nalgos=['MAAT','MVCC','TIMESTAMP','OCC','DLI_DTA3','DLI_OCC']
    nalgos=['MVCC']
    #nalgos=['NO_WAIT']
    npercpay=[0.0]
    # npercpay=[0.0]
    wh=128
    # wh=64
    # load = [1000,2000,3000,4000,5000]
    load = [5000]
    tcnt = [16]
    ctcnt = [16]
    fmt = ["WORKLOAD","NODE_CNT","CC_ALG","PERC_PAYMENT","NUM_WH","MAX_TXN_IN_FLIGHT","THREAD_CNT","CLIENT_THREAD_CNT"]
    exp = [[wl,n,cc,pp,wh*n,tif,thr,cthr] for thr,cthr,tif,pp,n,cc in itertools.product(tcnt,ctcnt,load,npercpay,nnodes,nalgos)]
#
    # wh=4
    # exp = exp+[[wl,n,cc,pp,wh*n,tif] for tif,pp,n,cc in itertools.product(load,npercpay,nnodes,nalgos)]
    return fmt,exp

def tpcc_cstress1():
    wl = 'TPCC'
    nnodes = [1]
    # nalgos=['NO_WAIT','WAIT_DIE','MAAT','MVCC','TIMESTAMP','CALVIN','WOOKONG']
    # nalgos=['NO_WAIT','WAIT_DIE','MAAT','MVCC','TIMESTAMP','OCC','CALVIN','WOOKONG','TICTOC','DLI_DTA','DLI_DTA1','DLI_DTA2','DLI_DTA3','DLI_MVCC_OCC','DLI_MVCC']
    # nalgos=['WOOKONG']
    nalgos=['MAAT','MVCC','TIMESTAMP','OCC','DLI_DTA3','DLI_OCC']
    #nalgos=['NO_WAIT']
    npercpay=[0.0]
    # npercpay=[0.0]
    wh=128
    # wh=64
    # load = [1000,2000,3000,4000,5000]
    load = [5000]
    tcnt = [16]
    ctcnt = [16]
    fmt = ["WORKLOAD","NODE_CNT","CC_ALG","PERC_PAYMENT","NUM_WH","MAX_TXN_IN_FLIGHT","THREAD_CNT","CLIENT_THREAD_CNT"]
    exp = [[wl,n,cc,pp,wh*n,tif,thr,cthr] for thr,cthr,tif,pp,n,cc in itertools.product(tcnt,ctcnt,load,npercpay,nnodes,nalgos)]

    # wh=4
    # exp = exp+[[wl,n,cc,pp,wh*n,tif] for tif,pp,n,cc in itertools.product(load,npercpay,nnodes,nalgos)]
    return fmt,exp

def tpcc_cstress2():
    wl = 'TPCC'
    nnodes = [1]
    # nalgos=['NO_WAIT','WAIT_DIE','MAAT','MVCC','TIMESTAMP','CALVIN','WOOKONG']
    # nalgos=['NO_WAIT','WAIT_DIE','MAAT','MVCC','TIMESTAMP','OCC','CALVIN','WOOKONG','TICTOC','DLI_DTA','DLI_DTA1','DLI_DTA2','DLI_DTA3','DLI_MVCC_OCC','DLI_MVCC']
    # nalgos=['MAAT','MVCC','TIMESTAMP','OCC','DLI_DTA3','DLI_OCC']
    # nalgos=['WOOKONG']
    nalgos=['DLI_DTA3','DLI_OCC']
    npercpay=[0.0]
    # npercpay=[0.0]
    wh=128
    # wh=64
    # load = [10000,20000,30000,40000,50000]
    load = [5000]
    #load = [1000]
    tcnt = [16]
    ctcnt = [16]
    fmt = ["WORKLOAD","NODE_CNT","CC_ALG","PERC_PAYMENT","NUM_WH","MAX_TXN_IN_FLIGHT","THREAD_CNT","CLIENT_THREAD_CNT"]
    exp = [[wl,n,cc,pp,wh*n,tif,thr,cthr] for thr,cthr,tif,pp,n,cc in itertools.product(tcnt,ctcnt,load,npercpay,nnodes,nalgos)]

    # wh=4
    # exp = exp+[[wl,n,cc,pp,wh*n,tif] for tif,pp,n,cc in itertools.product(load,npercpay,nnodes,nalgos)]
    return fmt,exp
# def tpcc_cstress2():
#     wl = 'YCSB'
#     nnodes = [2]
#     # nalgos=['NO_WAIT','WAIT_DIE','MAAT','MVCC','TIMESTAMP','CALVIN','WOOKONG']
#     # nalgos=['NO_WAIT','WAIT_DIE','MAAT','MVCC','TIMESTAMP','OCC','CALVIN','WOOKONG','TICTOC','DLI_DTA','DLI_DTA1','DLI_DTA2','DLI_DTA3','DLI_MVCC_OCC','DLI_MVCC']
#     nalgos=['MAAT','MVCC','TIMESTAMP','OCC','WOOKONG']
#     # npercpay=[0.0]
#     base_table_size=2097152*8
#     txn_write_perc = [0.5]
#     tup_write_perc = [0.5]
#     skew = [0.6]
#     # wh=64
#     # load = [10000,20000,30000,40000,50000]
#     load = [10000,20000]
#     tcnt = [16]
#     ctcnt = [16]
#     # fmt = ["WORKLOAD","NODE_CNT","CC_ALG","NUM_WH","MAX_TXN_IN_FLIGHT","THREAD_CNT","CLIENT_THREAD_CNT"]
#     # exp = [[wl,n,cc,wh*n,tif,thr,cthr] for thr,cthr,tif,n,cc in itertools.product(tcnt,ctcnt,load,nnodes,nalgos)]

#     fmt = ["WORKLOAD","NODE_CNT","CC_ALG","SYNTH_TABLE_SIZE","TUP_WRITE_PERC","TXN_WRITE_PERC","MAX_TXN_IN_FLIGHT","ZIPF_THETA","THREAD_CNT","CLIENT_THREAD_CNT"]
#     exp = [[wl,n,algo,base_table_size*n,tup_wr_perc,txn_wr_perc,ld,sk,thr,cthr] for thr,cthr,txn_wr_perc,tup_wr_perc,ld,n,sk,algo in itertools.product(tcnt,ctcnt,txn_write_perc,tup_write_perc,load,nnodes,skew,nalgos)]
#     # wh=4
#     # exp = exp+[[wl,n,cc,pp,wh*n,tif] for tif,pp,n,cc in itertools.product(load,npercpay,nnodes,nalgos)]
#     return fmt,exp
def tpcc_cstress3():
    wl = 'TPCC'
    nnodes = [1]
    # nalgos=['NO_WAIT','WAIT_DIE','MAAT','MVCC','TIMESTAMP','CALVIN','WOOKONG']
    nalgos=['NO_WAIT','WAIT_DIE','MAAT','MVCC','TIMESTAMP','OCC','CALVIN','WOOKONG','TICTOC','DLI_DTA','DLI_DTA1','DLI_DTA2','DLI_DTA3','DLI_MVCC_OCC','DLI_MVCC']
    # nalgos=['WOOKONG']
    #nalgos=['NO_WAIT']
    npercpay=[0.0]
    # npercpay=[0.0]
    wh=128
    # wh=64
    load = [1000,2000,3000,4000,5000]
    #load = [1000]
    tcnt = [60]
    ctcnt = [60]
    fmt = ["WORKLOAD","NODE_CNT","CC_ALG","PERC_PAYMENT","NUM_WH","MAX_TXN_IN_FLIGHT","THREAD_CNT","CLIENT_THREAD_CNT"]
    exp = [[wl,n,cc,pp,wh*n,tif,thr,cthr] for thr,cthr,tif,pp,n,cc in itertools.product(tcnt,ctcnt,load,npercpay,nnodes,nalgos)]

    # wh=4
    # exp = exp+[[wl,n,cc,pp,wh*n,tif] for tif,pp,n,cc in itertools.product(load,npercpay,nnodes,nalgos)]
    return fmt,exp

def tpcc_scaling1():
    wl = 'TPCC'
    nnodes = [1,2]
    nalgos=dta_target_algos
    npercpay=[0.0]
    wh=128
    load = [10000]
    fmt = ["WORKLOAD","NODE_CNT","CC_ALG","PERC_PAYMENT","NUM_WH","MAX_TXN_IN_FLIGHT"]
    exp = [[wl,n,cc,pp,wh*n,tif] for tif,pp,n,cc in itertools.product(load,npercpay,nnodes,nalgos)]
    return fmt,exp

def tpcc_stress1():
    wl = 'TPCC'
    nnodes = [1]
    nalgos=dta_target_algos
    npercpay=[0.0]
    wh=128
    load = tpcc_loads
    #load = tpcc_loads
    fmt = ["WORKLOAD","NODE_CNT","CC_ALG","PERC_PAYMENT","NUM_WH","MAX_TXN_IN_FLIGHT"]
    exp = [[wl,n,cc,pp,wh*n,tif] for tif,pp,n,cc in itertools.product(load,npercpay,nnodes,nalgos)]
    return fmt,exp

def tpcc_scaling_debug():
    wl = 'TPCC'
    nnodes = [1,2]
    nalgos=['WOOKONG']
    npercpay=[1.0]
    wh=32
    load = [20000]
    fmt = ["WORKLOAD","NODE_CNT","CC_ALG","PERC_PAYMENT","NUM_WH","MAX_TXN_IN_FLIGHT"]
    exp = [[wl,n,cc,pp,wh*n,tif] for tif,pp,n,cc in itertools.product(load,npercpay,nnodes,nalgos)]
    return fmt,exp

#def tpcc_scaling1():
#    wl = 'TPCC'
    # nnodes = [1,2,4,8,16,32]
#    nnodes = [32]
    # nalgos=['WOOKONG','MAAT','MVCC','TIMESTAMP']
#    nalgos=['WOOKONG']
#    npercpay=[0.0]
#    wh=32
#    load = [10000]
#    fmt = ["WORKLOAD","NODE_CNT","CC_ALG","PERC_PAYMENT","NUM_WH","MAX_TXN_IN_FLIGHT"]
#    exp = [[wl,n,cc,pp,wh*n,tif] for tif,pp,n,cc in itertools.product(load,npercpay,nnodes,nalgos)]
#    return fmt,exp

def tpcc_scaling2():
    wl = 'TPCC'
    nnodes = [1,2]
    # nnodes = [4]
    #nalgos=['WOOKONG','MAAT','MVCC','TIMESTAMP']
    nalgos=dta_target_algos
    npercpay=[1.0]
    wh=128
    load = [10000]
    fmt = ["WORKLOAD","NODE_CNT","CC_ALG","PERC_PAYMENT","NUM_WH","MAX_TXN_IN_FLIGHT"]
    exp = [[wl,n,cc,pp,wh*n,tif] for tif,pp,n,cc in itertools.product(load,npercpay,nnodes,nalgos)]
    return fmt,exp

def tpcc_stress2():
    wl = 'TPCC'
    nnodes = [1]
    # nnodes = [4]
    #nalgos=['WOOKONG','MAAT','MVCC','TIMESTAMP']
    #nalgos=['TIMESTAMP']
    nalgos=dta_target_algos
    npercpay=[1.0]
    wh=128
    load = tpcc_loads
    #load = tpcc_loads
    fmt = ["WORKLOAD","NODE_CNT","CC_ALG","PERC_PAYMENT","NUM_WH","MAX_TXN_IN_FLIGHT"]
    exp = [[wl,n,cc,pp,wh*n,tif] for tif,pp,n,cc in itertools.product(load,npercpay,nnodes,nalgos)]
    return fmt,exp

def tpcc_scaling3():
    wl = 'TPCC'
    nnodes = [1,2]
    # nnodes = [4]
    #nalgos=['WOOKONG','WAIT_DIE','MAAT','MVCC','TIMESTAMP','OCC']
    nalgos=dta_target_algos
    npercpay=[0.5]
    wh=128
    load = [10000]
    fmt = ["WORKLOAD","NODE_CNT","CC_ALG","PERC_PAYMENT","NUM_WH","MAX_TXN_IN_FLIGHT"]
    exp = [[wl,n,cc,pp,wh*n,tif] for tif,pp,n,cc in itertools.product(load,npercpay,nnodes,nalgos)]
    return fmt,exp

def tpcc_dist_ratio():
    wl = 'TPCC'
    nnodes = [16]
    nalgos=['WOOKONG','WAIT_DIE','MAAT','MVCC','TIMESTAMP','OCC']
    npercpay=[1.0]
    wh=32
    load = [10000]
    fmt = ["WORKLOAD","NODE_CNT","CC_ALG","PERC_PAYMENT","NUM_WH","MAX_TXN_IN_FLIGHT"]
    exp = [[wl,n,cc,pp,wh*n,tif] for tif,pp,n,cc in itertools.product(load,npercpay,nnodes,nalgos)]
    return fmt,exp


def tpcc_scaling_whset():
    wl = 'TPCC'
    nnodes = [1,2,4,8,16,32,64]
    nalgos=['NO_WAIT','WAIT_DIE','MAAT','MVCC','TIMESTAMP','CALVIN','WOOKONG']
    npercpay=[0.0,0.5,1.0]
    wh=128
    fmt = ["WORKLOAD","NODE_CNT","CC_ALG","PERC_PAYMENT","NUM_WH"]
    exp = [[wl,n,cc,pp,wh] for pp,n,cc in itertools.product(npercpay,nnodes,nalgos)]
    wh=256
    exp = exp + [[wl,n,cc,pp,wh] for pp,n,cc in itertools.product(npercpay,nnodes,nalgos)]
    return fmt,exp

def ycsb_skew_abort_writes():
    wl = 'YCSB'
    nnodes = [16]
    algos=['NO_WAIT','WAIT_DIE','MVCC','MAAT','CALVIN','TIMESTAMP','WOOKONG']
    base_table_size=2097152*8
    txn_write_perc = [0.0,0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9,1.0]
    tup_write_perc = [0.5]
    load = [10000]
    tcnt = [4]
    skew = [0.6]
    fmt = ["WORKLOAD","NODE_CNT","CC_ALG","SYNTH_TABLE_SIZE","TUP_WRITE_PERC","TXN_WRITE_PERC","MAX_TXN_IN_FLIGHT","ZIPF_THETA","THREAD_CNT","YCSB_ABORT_MODE"]
    exp = [[wl,n,algo,base_table_size*n,tup_wr_perc,txn_wr_perc,ld,sk,thr,'true'] for thr,txn_wr_perc,tup_wr_perc,ld,n,sk,algo in itertools.product(tcnt,txn_write_perc,tup_write_perc,load,nnodes,skew,algos)]
    return fmt,exp

def ycsb_skew_abort():
    wl = 'YCSB'
    nnodes = [16]
    algos=['NO_WAIT','WAIT_DIE','MVCC','MAAT','CALVIN','TIMESTAMP','WOOKONG']
    base_table_size=2097152*8
    txn_write_perc = [0.5]
    tup_write_perc = [0.5]
    load = [10000]
    tcnt = [4]
    skew = [0.0,0.25,0.5,0.55,0.6,0.65,0.7,0.75,0.8,0.825,0.85,0.875,0.9]
    fmt = ["WORKLOAD","NODE_CNT","CC_ALG","SYNTH_TABLE_SIZE","TUP_WRITE_PERC","TXN_WRITE_PERC","MAX_TXN_IN_FLIGHT","ZIPF_THETA","THREAD_CNT","YCSB_ABORT_MODE"]
    exp = [[wl,n,algo,base_table_size*n,tup_wr_perc,txn_wr_perc,ld,sk,thr,'true'] for thr,txn_wr_perc,tup_wr_perc,ld,n,sk,algo in itertools.product(tcnt,txn_write_perc,tup_write_perc,load,nnodes,skew,algos)]
    return fmt,exp


def ycsb_partitions_abort():
    wl = 'YCSB'
    nnodes = [16]
    algos=['NO_WAIT','WAIT_DIE','MVCC','MAAT','CALVIN','TIMESTAMP','WOOKONG']
    load = [10000]
    nparts = [1,2,4,6,8,10,12,14,16]
    base_table_size=2097152*8
    txn_write_perc = [0.5]
    tup_write_perc = [0.5]
    tcnt = [4]
    skew = [0.6]
    rpq =  16
    fmt = ["WORKLOAD","REQ_PER_QUERY","PART_PER_TXN","NODE_CNT","CC_ALG","SYNTH_TABLE_SIZE","TUP_WRITE_PERC","TXN_WRITE_PERC","MAX_TXN_IN_FLIGHT","ZIPF_THETA","THREAD_CNT","STRICT_PPT","YCSB_ABORT_MODE"]
    exp = [[wl,rpq,p,n,algo,base_table_size*n,tup_wr_perc,txn_wr_perc,ld,sk,thr,1,'true'] for thr,txn_wr_perc,tup_wr_perc,algo,sk,ld,n,p in itertools.product(tcnt,txn_write_perc,tup_write_perc,algos,skew,load,nnodes,nparts)]
    return fmt,exp

def network_sweep():
    wl = 'YCSB'
    nalgos=['NO_WAIT','WAIT_DIE','MVCC','MAAT','TIMESTAMP','CALVIN','WOOKONG']
    algos=['CALVIN']
# Network delay in ms
    ndelay=[0,0.05,0.1,0.25,0.5,0.75,1,1.75,2.5,5,7.5,10,17.5,25,50]
    ndelay = [int(n*1000000) for n in ndelay]
    nnodes = [2]
    txn_write_perc = [0.5]
    tup_write_perc = [0.5]
    load = [10000]
    tcnt = [4]
    skew = [0.6]
    base_table_size=2097152*8
    fmt = ["WORKLOAD","NODE_CNT","CC_ALG","SYNTH_TABLE_SIZE","TUP_WRITE_PERC","TXN_WRITE_PERC","MAX_TXN_IN_FLIGHT","ZIPF_THETA","THREAD_CNT","NETWORK_DELAY_TEST","NETWORK_DELAY","SET_AFFINITY"]
    exp = [[wl,n,algo,base_table_size*n,tup_wr_perc,txn_wr_perc,ld,sk,thr,"true",d,"false"] for thr,txn_wr_perc,tup_wr_perc,sk,ld,n,d,algo in itertools.product(tcnt,txn_write_perc,tup_write_perc,skew,load,nnodes,ndelay,nalgos)]
    return fmt,exp

##############################
# END PLOTS
##############################

experiment_map = {
    'pps_scaling': pps_scaling,
    'ycsb_scaling': ycsb_scaling,
    'ycsb_scaling1': ycsb_scaling1,
    'ycsb_scaling_abort': ycsb_scaling_abort,
    'ycsb_stress1': ycsb_stress1,
    'ycsb_stress2': ycsb_stress2,
    'ycsb_stress3': ycsb_stress3,
    'ycsb_stress4': ycsb_stress4,
    'ycsb_stress5': ycsb_stress5,
    'ycsb_stress6': ycsb_stress6,
    'ycsb_stress7': ycsb_stress7,
    'ycsb_stress8': ycsb_stress8,
    'ycsb_stress9': ycsb_stress9,
    'ycsb_stress10': ycsb_stress10,
    'ycsb_stress11': ycsb_stress11,
    'ycsb_stress12': ycsb_stress12,
    'ppr_ycsb_scaling_abort': ycsb_scaling_abort,
    'ppr_ycsb_scaling_abort_plot': ppr_ycsb_scaling_abort_plot,
    'ycsb_writes': ycsb_writes,
    'ycsb_skew': ycsb_skew,
    'ycsb_skew1': ycsb_skew1,
    'isolation_levels': isolation_levels,
    'ycsb_partitions': ycsb_partitions,
    'ycsb_partitions_abort': ycsb_partitions_abort,
    'ppr_ycsb_partitions_abort': ycsb_partitions_abort,
    'ppr_ycsb_partitions_abort_plot': ppr_ycsb_partitions_abort_plot,
    'tpcc_scaling': tpcc_scaling,
    'tpcc_cstress':tpcc_cstress,
    'tpcc_cstress1':tpcc_cstress1,
    'tpcc_cstress2':tpcc_cstress2,
    'tpcc_cstress3':tpcc_cstress3,
    'tpcc_scaling_debug': tpcc_scaling_debug,
    'tpcc_scaling1': tpcc_scaling1,
    'tpcc_scaling2': tpcc_scaling2,
    'tpcc_scaling3': tpcc_scaling3,
    'tpcc_stress1': tpcc_stress1,
    'tpcc_stress2': tpcc_stress2,
    'tpcc_dist_ratio': tpcc_dist_ratio,
    'tpcc_scaling_whset': tpcc_scaling_whset,
    'ycsb_skew_abort': ycsb_skew_abort,
    'ppr_pps_scaling': pps_scaling,
    'ppr_pps_scaling_plot': ppr_pps_scaling_plot,
    'ppr_ycsb_scaling': ycsb_scaling,
    'ppr_ycsb_scaling_plot': ppr_ycsb_scaling_plot,
    'ecwc': ecwc,
    'ppr_ecwc': ecwc,
    'ppr_ecwc_plot': ppr_ecwc_plot,
    'ppr_ycsb_skew': ycsb_skew,
    'ppr_ycsb_skew_plot': ppr_ycsb_skew_plot,
    'ppr_ycsb_skew_abort': ycsb_skew_abort,
    'ppr_ycsb_skew_abort_plot': ppr_ycsb_skew_abort_plot,
    'ppr_ycsb_writes': ycsb_writes,
    'ppr_ycsb_writes_plot': ppr_ycsb_writes_plot,
    'ppr_ycsb_partitions': ycsb_partitions,
    'ppr_ycsb_partitions_plot': ppr_ycsb_partitions_plot,
    'ppr_isolation_levels': isolation_levels,
    'ppr_isolation_levels_plot': ppr_isolation_levels_plot,
    'ppr_tpcc_scaling': tpcc_scaling,
    'ppr_tpcc_scaling_plot': ppr_tpcc_scaling_plot,
    'network_sweep' : network_sweep,
    'ppr_network' : network_sweep,
    'ppr_network_plot' : ppr_network_plot,
}

# UPDATE!
# Default values for variable configurations

configs = {
  ##TraversalActionSequenceCreator
  "TRANS_CNT" : 2,
  "ITEM_CNT" : 4,
  "SUBTASK_NUM" : 1,
  "SUBTASK_ID" : 0,
  "MAX_DML" : 4,
  "WITH_ABORT" : "false",
  "TAIL_DTL" : "false",
  "SAVE_HISTROY_WITH_EMPTY_OPT" : "false",
  "DYNAMIC_SEQ_LEN" : "false",

  ##InputActionSequenceCreator
  ## ! Parameters used to locate distributed performance bottlenecks
  "SECOND" : 100,
  "ONE_NODE_RECIEVE" : 0,
  "TXN_QUEUE_PERCENT" : 0.0,
  "SEND_TO_SELF_PAHSE" : 0,
  "MALLOC_TYPE" : 0,
  "SEND_STAGE" : 1,

  ## Simulation + Hardware
  "NODE_CNT" : 2,
  "THREAD_CNT":16,
  "REM_THREAD_CNT": 8,
  "SEND_THREAD_CNT": 8,
  "MIG_THREAD_CNT" : 1,## migrate thread
  "CORE_CNT" : 2,
  ## PART_CNT should be at least NODE_CNT
  "PART_CNT" : "NODE_CNT * 2",
  "CLIENT_NODE_CNT" : 1,
  "CLIENT_THREAD_CNT" : 2,
  "CLIENT_REM_THREAD_CNT" : 2,
  "CLIENT_SEND_THREAD_CNT" : 2,
  "CLIENT_RUNTIME" : "false",

  ##key_to_part HASH_MODE: key_to_part(key) = key % g_part_cnt    CONST_MODE: key_to_part(key) = key / (SYNTH_TABLE_SIZE / g_part_cnt)
  "KEY_TO_PART" : "HASH_MODE",
  "HASH_MODE" : 0,
  "CONST_MODE" : 1,

  ##PART_TO_NODE 
  "PART_TO_NODE" : "KEY_TO_PART",
  "LOAD_METHOD" : "LOAD_MAX",
  "LOAD_PER_SERVER" : 100,

  ## MIG ALG : DETEST REMUS LOCK
  "DETEST" : 0,
  "REMUS" : 1,
  "LOCK" : 2,
  "MIGRATION_ALG" : "LOCK",

  ##DETEST Migration
  "PART_SPLIT_CNT" : 2,
  "PART_HOT_CNT" : 2,
  ##Remus Migration
  "SYNCTIME" : 1,
  ##MIGRATION
  "MIGRATION" : 'true',

  ##Replication
  "REPLICA_CNT" : 0,
  ##AA (Active-Active), AP (Active-Passive)
  "REPL_TYPE" : "AP",

  ##each transaction only accesses only 1 virtual partition. But the lock/ts manager and index are
  ##not aware of such partitioning. VIRTUAL_PART_CNT describes the request distribution and is only
  ##used to generate queries. For HSTORE, VIRTUAL_PART_CNT should be the same as PART_CNT.
  "VIRTUAL_PART_CNT" : "PART_CNT",
  "PAGE_SIZE" : 4096,
  "CL_SIZE" : 64,
  "CPU_FREQ" : 2.6,
  "HW_MIGRATE" : "false",

  ## of transactions to run for warmup
  "WARMUP" : 0,
  ##YCSB or TPCC or PPS or DA
  "WORKLOAD" : "YCSB",
  ##print the transaction latency distribution
  "PRT_LAT_DISTR" : "false",
  "STATS_ENABLE" : "true",
  "TIME_ENABLE" : "true", ##STATS_ENABLE

  "FIN_BY_TIME" : 'true',
  "MAX_TXN_IN_FLIGHT" : 1000000,

  "SERVER_GENERATE_QUERIES" : 'false',

  ##Memory System

  ##Three different memory allocation methods are supported.
  ## 1. default libc malloc
  ## 2. per-thread malloc. each thread has a private local memory
  ##    pool
  ## 3. per-partition malloc. each partition has its own memory pool
  ##    which is mapped to a unique tile on the chip.
  "MEM_ALLIGN" : 8,

  ##[THREAD_ALLOC]
  "THREAD_ALLOC" : "false",
  ##"THREAD_ARENA_SIZE" :     (1UL << 22)
  "MEM_PAD" : "true",

  ##[PART_ALLOC]
  "PART_ALLOC" : 'true',
  ##"MEM_SIZE"           (1UL << 30)
  "NO_FREE" : 'false',

  ##// Message Passing
  "TPORT_TYPE" : 'tcp',
  "TPORT_PORT" : 7000,
  "SET_AFFINITY" : 'true',

  "MAX_TPORT_NAME" : 128,
  "MSG_SIZE" : 128,## // in bytes
  "HEADER_SIZE" : "sizeof(uint32_t)*2 // in bits",
  "MSG_TIMEOUT" : "5000000000UL // in ns",
  "NETWORK_TEST" : 'false',
  "NETWORK_DELAY_TEST" : 'false',
  "NETWORK_DELAY" : '0UL',

  "MAX_QUEUE_LEN" : 'NODE_CNT',
  "PRIORITY_WORK_QUEUE" : 'false',
  "PRIORITY" : 'PRIORITY_ACTIVE',
  "MSG_SIZE_MAX" : 134217728,
  "MSG_TIME_LIMIT" : 0,

  "SIM_FULL_ROW" : 'true',

  ##Concurrency Control
  ##WAIT_DIE, NO_WAIT, TIMESTAMP, MVCC, CALVIN, MAAT, WOOKONG, TICTOC, SI
  "ISOLATION_LEVEL" : 'SERIALIZABLE',
  "CC_ALG" : 'SSI',
  "YCSB_ABORT_MODE" : 'false',
  "QUEUE_CAPACITY_NEW" : 1000000,
  ##all transactions acquire tuples according to the primary key order.
  "KEY_ORDER" : 'false',
  ##transaction roll back changes after abort
  "ROLL_BACK" : 'true',
  ##per-row lock/ts management or central lock/ts management
  "CENTRAL_MAN" : 'false',
  "BUCKET_CNT" : 31,
  "ABORT_PENALTY" : '10 * 1000000UL   // in ns.',
  "ABORT_PENALTY_MAX" : '5 * 100 * 1000000UL   // in ns.',
  "BACKOFF" : 'true',
  ##[ INDEX ]
  "ENABLE_LATCH" : 'false',
  "CENTRAL_INDEX" : 'false',
  "CENTRAL_MANAGER" : 'false',
  "INDEX_STRUCT" : 'IDX_BTREE',
  "BTREE_ORDER" : 4,

  ##[TIMESTAMP]
  "TS_TWR" : 'false',
  "TS_ALLOC" : 'TS_CLOCK',
  "TS_BATCH_ALLOC" : 'false',
  "TS_BATCH_NUM" : 1,
  ##[MVCC]
  ##when read/write history is longer than HIS_RECYCLE_LEN
  ##the history should be recycled.
  "HIS_RECYCLE_LEN" : 10,
  "MAX_PRE_REQ" : 'MAX_TXN_IN_FLIGHT * NODE_CNT//1024',
  "MAX_READ_REQ" : 'MAX_TXN_IN_FLIGHT * NODE_CNT//1024',
  "MIN_TS_INTVL" : '10 * 1000000UL // 10ms',
  ##[OCC]
  "MAX_WRITE_SET" : 10,
  "PER_ROW_VALID" : 'false',
  ##[VLL]
  "TXN_QUEUE_SIZE_LIMIT" : 'THREAD_CNT',
  ##[CALVIN]
  "SEQ_THREAD_CNT" : 4,
  ##[TICTOC]
  "MAX_NUM_WAITS" : 4,
  "PRE_ABORT" : 'true',
  "OCC_LOCK_TYPE" : 'WAIT_DIE',
  "OCC_WAW_LOCK" : 'true',



  ## Benchmark
  ##max number of rows touched per transaction
  "MAX_ROW_PER_TXN" : 65536,
  "QUERY_INTVL" : '1UL',
  "MAX_TXN_PER_PART" : 100000,
  "FIRST_PART_LOCAL" : 'false',
  "M0AX_TUPLE_SIZE" : '512 // in bytes',
  "GEN_BY_MPR" : 'false',

  ##==== [YCSB] ====
  ##SKEW_METHOD:
  ##ZIPF: use ZIPF_THETA distribution
  ##HOT: use ACCESS_PERC of the accesses go to DATA_PERC of the data
  "SKEW_METHOD" : 'ZIPF',
  "DATA_PERC" : '(SYNTH_TABLE_SIZE / 64)',
  "ACCESS_PERC" : 0.3,
  "INIT_PARALLELISM" : '(PART_CNT / NODE_CNT)',
  "SYNTH_TABLE_SIZE" : 65536,
  "ZIPF_THETA" : 0.6,
  "TXN_WRITE_PERC" : 0.7,
  "TUP_WRITE_PERC" : 0.3,
  "SCAN_PERC" : 0,
  "SCAN_LEN" : 20,
  "PART_PER_TXN" : 2,
  "PERC_MULTI_PART" : 'MPR',
  "REQ_PER_QUERY" : 10,
  "FIELD_PER_TUPLE" : 10,
  "CREATE_TXN_FILE" : 'false',
  "STRICT_PPT" : 0,
  
  ##==== [TPCC] ====
  ##For large warehouse count, the tables do not fit in memory
  ##small tpcc schemas shrink the table size.
  "TPCC_SMALL" : 'false',
  "MAX_ITEMS_SMALL" : 10000,
  "CUST_PER_DIST_SMALL" : 2000,
  "MAX_ITEMS_NORM" : 100000,
  "CUST_PER_DIST_NORM" : 3000,
  "MAX_ITEMS_PER_TXN" : 15,
  ##Some of the transactions read the data but never use them.
  ##If TPCC_ACCESS_ALL == fales, then these parts of the transactions
  ##are not modeled.
  "TPCC_ACCESS_ALL" : 'false',
  "WH_UPDATE" : 'true',
  "NUM_WH" : 'PART_CNT',
  ## % of transactions that access multiple partitions
  "MPR" : 1.0,
  "MPIR" : 0.01,
  "MPR_NEWORDER" : '20 // In %',

  ##DEBUG info
  "WL_VERB" : 'true',
  "IDX_VERB" : 'false',
  "VERB_ALLOC" : 'true',

  "DEBUG_LOCK" : 'false',
  "DEBUG_TIMESTAMP" : 'false',
  "DEBUG_SYNTH" : 'false',
  "DEBUG_ASSERT" : 'false',
  "DEBUG_DISTR" : 'false',
  "DEBUG_ALLOC" : 'false',
  "DEBUG_RACE" : 'false',
  "DEBUG_TIMELINE" : 'false',
  "DEBUG_BREAKDOWN" : 'false',
  "DEBUG_LATENCY" : 'false',

  ## MODES
  ##QRY Only do query operations, no 2PC
  ##TWOPC Only do 2PC, no query work
  ##SIMPLE Immediately send OK back to client
  ##NOCC Don't do CC
  ##NORMAL normal operation
  "MODE" : 'NORMAL_MODE',

  ##Constant
  ##INDEX_STRUCT
  "IDX_HASH" : 1,
  "IDX_BTREE" : 2,
  ##WORKLOAD
  "YCSB" : 1,
  "TPCC":            2,
  "PPS":             3,
  "TEST":            4,
  "DA": 5,
  ##Concurrency Control Algorithm
  "NO_WAIT":           1,
  "WAIT_DIE":          2,
  "DL_DETECT":         3,
  "TIMESTAMP":         4,
  "MVCC":              5,
  "HSTORE":            6,
  "HSTORE_SPEC":           7,
  "OCC":             8,
  "VLL":             9,
  "CALVIN":      10,
  "MAAT":      11,
  "WDL":           12,
  "WOOKONG":     13,
  "TICTOC":     14,
  "FOCC":       15,
  "BOCC":       16,
  "SSI":        17,
  "WSI":        18,
  "DLI_BASE": 19,
  "DLI_OCC": 20,
  "DLI_MVCC_OCC": 21,
  "DTA": 22,
  "DLI_DTA": 23,
  "DLI_MVCC": 24,
  "DLI_DTA2": 25,
  "DLI_DTA3": 26,
  "SILO": 27,
  "CNULL": 28,
  ## TIMESTAMP allocation method.
  "TS_MUTEX":          1,
  "TS_CAS":            2,
  "TS_HW":           3,
  "TS_CLOCK":          4,
  "TS_CURL_CLOCK":          5,
  "LTS_TCP_CLOCK":          6,
  "LTS_HLC_CLOCK":          7,

  "LTS_TCP_IP":  "10.77.110.147",
  "LTS_TCP_PORT" : 62389,

  ## MODES
  ## NORMAL < NOCC < QRY_ONLY < SETUP < SIMPLE
  "NORMAL_MODE": 1,
  "NOCC_MODE": 2,
  "QRY_ONLY_MODE": 3,
  "SETUP_MODE": 4,
  "SIMPLE_MODE": 5,
  ## SKEW METHODS
  "ZIPF": 1,
  "HOT": 2,
  ## PRIORITY WORK QUEUE
  "PRIORITY_FCFS": 1,
  "PRIORITY_ACTIVE" : 2,
  "PRIORITY_HOME": 3,
  ## Replication
  "AA" : 1,
  "AP" : 2,
  ## Load
  "LOAD_MAX" : 1,
  "LOAD_RATE" : 2,
  ## Transport
  "TCP": 1,
  "IPC": 2,
  ## Isolation levels
  "SERIALIZABLE" : 1,
  "READ_COMMITTED": 2,
  "READ_UNCOMMITTED" : 3,
  "NOLOCK" : 4,

  ## Stats and timeout
  "BILLION" : '1000000000UL // in ns => 1 second',
  "MILLION" : '1000000UL // in ns => 1 second',
  "STAT_ARR_SIZE" : 1024,
  "PROG_TIMER" : '10 * BILLION // in s',
  "BATCH_TIMER" : 0,
  "SEQ_BATCH_TIMER" : '5 * 1 * MILLION // ~5ms -- same as CALVIN paper',
  "DONE_TIMER" : '1 * 39 * BILLION // ~1 minutes  60 BILLION = 1 min',
  "WARMUP_TIMER" : '1 * 1 * BILLION // ~1 minutes',

  "SEED" : 0,
  "SHMEM_ENV": 'false',
  "ENVIRONMENT_EC2": 'false'
}