
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
  "INPUT_FILE_PATH" : "../input.txt",
  ## ! Parameters used to locate distributed performance bottlenecks
  "SECOND" : 100,
  "ONE_NODE_RECIEVE" : 0,
  "TXN_QUEUE_PERCENT" : 0.0,
  "SEND_TO_SELF_PAHSE" : 0,
  "MALLOC_TYPE" : 0,
  "SEND_STAGE" : 1,

  ## Simulation + Hardware
  "NODE_CNT" : 2,
  "THREAD_CNT": 4,
  "REM_THREAD_CNT": 2,
  "SEND_THREAD_CNT": 2,
  "MIG_THREAD_CNT" : 1,## migrate thread
  "CORE_CNT" : 2,
  ## PART_CNT should be at least NODE_CNT
  "PART_CNT" : "NODE_CNT" * 4,
  "CLIENT_NODE_CNT" : 1,
  "CLIENT_THREAD_CNT" : 4,
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
  "MAX_TXN_IN_FLIGHT" : 10000,

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
  "NETWORK_DELAY_TEST" : 'true',
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

  """
  // [SILO]
  #define VALIDATION_LOCK				"no-wait" // no-wait or waiting
  #define PRE_ABORT2					"true"
  #define ATOMIC_WORD					false
  /***********************************************/
  // Logging
  /***********************************************/
  #define LOG_COMMAND         false
  #define LOG_REDO          false
  #define LOGGING false
  #define LOG_BUF_MAX 10
  #define LOG_BUF_TIMEOUT 10 * 1000000UL // 10ms
  """


  ## Benchmark
  ##max number of rows touched per transaction
  "MAX_ROW_PER_TXN" : 65536,
  "QUERY_INTVL" : '1UL',
  "MAX_TXN_PER_PART" : 1000000,
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
'''
enum TPCCTable {
  TPCC_WAREHOUSE,
          TPCC_DISTRICT,
          TPCC_CUSTOMER,
          TPCC_HISTORY,
          TPCC_NEWORDER,
          TPCC_ORDER,
          TPCC_ORDERLINE,
          TPCC_ITEM,
  TPCC_STOCK
};
enum TPCCTxnType {
  TPCC_ALL,
          TPCC_PAYMENT,
          TPCC_NEW_ORDER,
          TPCC_ORDER_STATUS,
          TPCC_DELIVERY,
  TPCC_STOCK_LEVEL
};
enum DATxnType {
  DA_READ,
  DA_WRITE,
  DA_COMMIT,
  DA_ABORT,
  DA_SCAN
};
#define MAX_DA_TABLE_SIZE 10000


extern TPCCTxnType g_tpcc_txn_type;
//#define TXN_TYPE          TPCC_ALL
#define PERC_PAYMENT 0.0
#define FIRSTNAME_MINLEN      8
#define FIRSTNAME_LEN         16
#define LASTNAME_LEN        16

#define DIST_PER_WH       10

// PPS (Product-Part-Supplier)
#define MAX_PPS_PARTS_PER 10
#define MAX_PPS_PART_KEY 10000
#define MAX_PPS_PRODUCT_KEY 1000
#define MAX_PPS_SUPPLIER_KEY 1000
#define MAX_PPS_PART_PER_PRODUCT 10
#define MAX_PPS_PART_PER_SUPPLIER 10
#define MAX_PPS_PART_PER_PRODUCT_KEY 10
#define MAX_PPS_PART_PER_SUPPLIER_KEY 10

#define PERC_PPS_GETPART 0.00
#define PERC_PPS_GETSUPPLIER 0.00
#define PERC_PPS_GETPRODUCT 0.0
#define PERC_PPS_GETPARTBYSUPPLIER 0.0
#define PERC_PPS_GETPARTBYPRODUCT 0.2
#define PERC_PPS_ORDERPRODUCT 0.6
#define PERC_PPS_UPDATEPRODUCTPART 0.2
#define PERC_PPS_UPDATEPART 0.0

enum PPSTxnType {
  PPS_ALL = 0,
          PPS_GETPART,
          PPS_GETSUPPLIER,
          PPS_GETPRODUCT,
          PPS_GETPARTBYSUPPLIER,
          PPS_GETPARTBYPRODUCT,
          PPS_ORDERPRODUCT,
          PPS_UPDATEPRODUCTPART,
          PPS_UPDATEPART
          };
'''


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
  "DONE_TIMER" : '1 * 30 * BILLION // ~1 minutes  60 BILLION = 1 min',
  "WARMUP_TIMER" : '1 * 20 * BILLION // ~1 minutes',

  "SEED" : 0,
  "SHMEM_ENV": 'false',
  "ENVIRONMENT_EC2": 'false'
}

