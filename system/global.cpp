/*
   Copyright 2016 Massachusetts Institute of Technology

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#include "global.h"
#include "mem_alloc.h"
#include "stats.h"
#include "sim_manager.h"
#include "manager.h"
#include "query.h"
#include "client_query.h"
#include "occ.h"
#include "bocc.h"
#include "focc.h"
#include "ssi.h"
#include "wsi.h"
#include "transport.h"
#include "work_queue.h"
#include "abort_queue.h"
#include "client_query.h"
#include "client_txn.h"
#include "logger.h"
#include "maat.h"
#include "manager.h"
#include "mem_alloc.h"
#include "msg_queue.h"
#include "pool.h"
#include "query.h"
#include "sequencer.h"
#include "dli.h"
#include "sim_manager.h"
#include "stats.h"
#include "transport.h"
#include "txn_table.h"
#include "work_queue.h"
#include "dta.h"
#include "client_txn.h"
#include "sequencer.h"
#include "logger.h"
#include "maat.h"
#include "wkdb.h"
#include "tictoc.h"
#include "key_xid.h"
#include "rts_cache.h"
#include "migmsg_queue.h"

#include <boost/lockfree/queue.hpp>
#include "da_block_queue.h"


mem_alloc mem_allocator;
Stats stats;
SimManager * simulation;
Manager glob_manager;
Query_queue query_queue;
Client_query_queue client_query_queue;
OptCC occ_man;
Dli dli_man;
Focc focc_man;
Bocc bocc_man;
Maat maat_man;
Dta dta_man;
Wkdb wkdb_man;
ssi ssi_man;
wsi wsi_man;
Tictoc tictoc_man;
Transport tport_man;
TxnManPool txn_man_pool;
TxnPool txn_pool;
AccessPool access_pool;
TxnTablePool txn_table_pool;
MsgPool msg_pool;
RowPool row_pool;
QryPool qry_pool;
TxnTable txn_table;
QWorkQueue work_queue;
AbortQueue abort_queue;
MessageQueue msg_queue;
MigrateMessageQueue migmsg_queue;
Client_txn client_man;
Sequencer seq_man;
Logger logger;
TimeTable time_table;
DtaTimeTable dta_time_table;
KeyXidCache dta_key_xid_cache;
RtsCache dta_rts_cache;
InOutTable inout_table;
WkdbTimeTable wkdb_time_table;
KeyXidCache wkdb_key_xid_cache;
RtsCache wkdb_rts_cache;
// QTcpQueue tcp_queue;

boost::lockfree::queue<DAQuery*, boost::lockfree::fixed_sized<true>> da_query_queue{100};
DABlockQueue da_gen_qry_queue(50);
bool is_server=false;
map<uint64_t, ts_t> da_start_stamp_tab;
set<uint64_t> da_start_trans_tab;
map<uint64_t, ts_t> da_stamp_tab;
set<uint64_t> already_abort_tab;
string DA_history_mem;
bool abort_history;
ofstream commit_file;
ofstream abort_file;

bool volatile warmup_done = false;
bool volatile enable_thread_mem_pool = false;
pthread_barrier_t warmup_bar;

ts_t g_abort_penalty = ABORT_PENALTY;
ts_t g_abort_penalty_max = ABORT_PENALTY_MAX;
bool g_central_man = CENTRAL_MAN;
UInt32 g_ts_alloc = TS_ALLOC;
bool g_key_order = KEY_ORDER;
bool g_ts_batch_alloc = TS_BATCH_ALLOC;
UInt32 g_ts_batch_num = TS_BATCH_NUM;
int32_t g_inflight_max = MAX_TXN_IN_FLIGHT;
//int32_t g_inflight_max = MAX_TXN_IN_FLIGHT/NODE_CNT;
uint64_t g_msg_size = MSG_SIZE_MAX;
int32_t g_load_per_server = LOAD_PER_SERVER;

bool g_hw_migrate = HW_MIGRATE;

int node_inflight_max[NODE_CNT]; 

bool g_migrate_flag = MIGRATION;
uint64_t g_mig_starttime;
uint64_t g_mig_endtime;

volatile UInt64 g_row_id = 0;
bool g_part_alloc = PART_ALLOC;
bool g_mem_pad = MEM_PAD;
UInt32 g_cc_alg = CC_ALG;
ts_t g_query_intvl = QUERY_INTVL;
UInt32 g_part_per_txn = PART_PER_TXN;
double g_perc_multi_part = PERC_MULTI_PART;
double g_txn_read_perc = 1.0 - TXN_WRITE_PERC;
double g_txn_write_perc = TXN_WRITE_PERC;
double g_tup_read_perc = 1.0 - TUP_WRITE_PERC;
double g_tup_write_perc = TUP_WRITE_PERC;
double g_zipf_theta = ZIPF_THETA;
double g_data_perc = DATA_PERC;
double g_access_perc = ACCESS_PERC;
bool g_prt_lat_distr = PRT_LAT_DISTR;
UInt32 g_node_id = 0;
UInt32 g_node_cnt = NODE_CNT;
UInt32 g_part_cnt = PART_CNT;
UInt32 g_part_split_cnt = PART_SPLIT_CNT;
UInt32 g_virtual_part_cnt = VIRTUAL_PART_CNT;
UInt32 g_core_cnt = CORE_CNT;

#if CC_ALG == HSTORE || CC_ALG == HSTORE_SPEC
UInt32 g_thread_cnt = PART_CNT/NODE_CNT;
#else
UInt32 g_thread_cnt = THREAD_CNT;
#endif
UInt32 g_rem_thread_cnt = REM_THREAD_CNT;
UInt32 g_abort_thread_cnt = 1;
#if LOGGING
UInt32 g_logger_thread_cnt = 1;
#else
UInt32 g_logger_thread_cnt = 0;
#endif
UInt32 g_send_thread_cnt = SEND_THREAD_CNT;
#if MIGRATION
UInt32 g_migrate_thread_cnt = MIG_THREAD_CNT;
#else
UInt32 g_migrate_thread_cnt = 0;
#endif
UInt32 g_stat_thread_cnt = 1;
#if CC_ALG == CALVIN
// sequencer + scheduler thread
UInt32 g_total_thread_cnt = g_thread_cnt + g_rem_thread_cnt + g_send_thread_cnt + g_abort_thread_cnt + g_logger_thread_cnt + 3;
#else
    #if MIGRATION
        UInt32 g_total_thread_cnt = g_thread_cnt + g_rem_thread_cnt + g_send_thread_cnt + g_abort_thread_cnt + g_logger_thread_cnt + g_migrate_thread_cnt + g_stat_thread_cnt + 1;
    #else
        UInt32 g_total_thread_cnt = g_thread_cnt + g_rem_thread_cnt + g_send_thread_cnt + g_abort_thread_cnt + g_logger_thread_cnt + g_stat_thread_cnt + 1;
    #endif
#endif

UInt32 g_total_client_thread_cnt = g_client_thread_cnt + g_client_rem_thread_cnt + g_client_send_thread_cnt;
UInt32 g_total_node_cnt = g_node_cnt + g_client_node_cnt + g_repl_cnt*g_node_cnt;
UInt64 g_synth_table_size = SYNTH_TABLE_SIZE;
UInt32 g_req_per_query = REQ_PER_QUERY;
bool g_strict_ppt = STRICT_PPT == 1;
UInt32 g_field_per_tuple = FIELD_PER_TUPLE;
UInt32 g_init_parallelism = INIT_PARALLELISM;
UInt32 g_client_node_cnt = CLIENT_NODE_CNT;
UInt32 g_client_thread_cnt = CLIENT_THREAD_CNT;
UInt32 g_client_rem_thread_cnt = CLIENT_REM_THREAD_CNT;
UInt32 g_client_send_thread_cnt = CLIENT_SEND_THREAD_CNT;
UInt32 g_servers_per_client = g_node_cnt;
UInt32 g_clients_per_server = 0;
UInt32 g_server_start_node = 0;
vector<int> query_to_part(g_part_cnt);
vector<int> query_to_row(g_synth_table_size);
vector<int> query_to_minipart(PART_SPLIT_CNT);
vector< pair<uint64_t, uint64_t> > edge_index;
double co_access[PART_SPLIT_CNT][PART_SPLIT_CNT];
vector<double> wtime = {}; //by cost model
vector<double> wlatency = {};


UInt32 g_this_thread_cnt = ISCLIENT ? g_client_thread_cnt : g_thread_cnt;
UInt32 g_this_rem_thread_cnt = ISCLIENT ? g_client_rem_thread_cnt : g_rem_thread_cnt;
UInt32 g_this_send_thread_cnt = ISCLIENT ? g_client_send_thread_cnt : g_send_thread_cnt;
UInt32 g_this_total_thread_cnt = ISCLIENT ? g_total_client_thread_cnt : g_total_thread_cnt;

UInt32 g_max_txn_per_part = MAX_TXN_PER_PART;
UInt32 g_network_delay = NETWORK_DELAY;
UInt64 g_done_timer = DONE_TIMER;
UInt64 g_batch_time_limit = BATCH_TIMER;
UInt64 g_seq_batch_time_limit = SEQ_BATCH_TIMER;
UInt64 g_prog_timer = PROG_TIMER;
UInt64 g_warmup_timer = WARMUP_TIMER;
UInt64 g_msg_time_limit = MSG_TIME_LIMIT;
UInt64 g_starttime=0;

UInt64 g_log_buf_max = LOG_BUF_MAX;
UInt64 g_log_flush_timeout = LOG_BUF_TIMEOUT;

// MVCC
UInt64 g_max_read_req = MAX_READ_REQ;
UInt64 g_max_pre_req = MAX_PRE_REQ;
UInt64 g_his_recycle_len = HIS_RECYCLE_LEN;

// CALVIN
UInt32 g_seq_thread_cnt = SEQ_THREAD_CNT;

// TICTOC
uint32_t g_max_num_waits = MAX_NUM_WAITS;

double g_mpr = MPR;
double g_mpitem = MPIR;

// PPS (Product-Part-Supplier)
UInt32 g_max_parts_per = MAX_PPS_PARTS_PER;
UInt32 g_max_part_key = MAX_PPS_PART_KEY;
UInt32 g_max_product_key = MAX_PPS_PRODUCT_KEY;
UInt32 g_max_supplier_key = MAX_PPS_SUPPLIER_KEY;
double g_perc_getparts = PERC_PPS_GETPART;
double g_perc_getproducts = PERC_PPS_GETPRODUCT;
double g_perc_getsuppliers = PERC_PPS_GETSUPPLIER;
double g_perc_getpartbyproduct = PERC_PPS_GETPARTBYPRODUCT;
double g_perc_getpartbysupplier = PERC_PPS_GETPARTBYSUPPLIER;
double g_perc_orderproduct = PERC_PPS_ORDERPRODUCT;
double g_perc_updateproductpart = PERC_PPS_UPDATEPRODUCTPART;
double g_perc_updatepart = PERC_PPS_UPDATEPART;

// TPCC
UInt32 g_num_wh = NUM_WH;
double g_perc_payment = PERC_PAYMENT;
bool g_wh_update = WH_UPDATE;
char * output_file = NULL;
char * input_file = NULL;
char * txn_file = NULL;

#if TPCC_SMALL
UInt32 g_max_items = MAX_ITEMS_SMALL;
UInt32 g_cust_per_dist = CUST_PER_DIST_SMALL;
//uint64_t g_tuplesize[9] = {105, 121, 78, 56, 24, 64, 40, 98, 32}; 
uint64_t g_tuplesize[7] = {105, 121, 1024 * 5, 56, 64, 98, 32}; //只统计迁移表
//warehouse, distinct, customer, history, order, item, stock
#else
UInt32 g_max_items = MAX_ITEMS_NORM;
UInt32 g_cust_per_dist = CUST_PER_DIST_NORM;
uint64_t g_tuplesize[9] = {105, 121, 703, 80, 24, 64, 80, 98, 338};
#endif
UInt32 g_max_items_per_txn = MAX_ITEMS_PER_TXN;
UInt32 g_dist_per_wh = DIST_PER_WH;

UInt32 g_repl_type = REPL_TYPE;
UInt32 g_repl_cnt = REPLICA_CNT;

map<string, string> g_params;

double percents[TPS_LENGTH];

uint64_t get_node_id_mini(uint64_t key){
  #if MIGRATION_ALG == DETEST
    if (key_to_part(key) != MIGRATION_PART) return GET_NODE_ID(key_to_part(key));
    else {
      return get_minipart_node_id(get_minipart_id(key));
    }
  #elif MIGRATION_ALG == SQUALL
    if (key_to_part(key) != MIGRATION_PART) return GET_NODE_ID(key_to_part(key));
    else {
      return get_squallpart_node_id(get_squallpart_id(key));
    }
  #elif MIGRATION_ALG == DETEST_SPLIT
    if (key_to_part(key) != 0) return GET_NODE_ID(key_to_part(key));
    else {
      return row_map[key][0];
    }

  #else
    return GET_NODE_ID(key_to_part(key));
  #endif
}


std::map <uint64_t, std::vector<uint64_t>> part_map;
uint64_t remus_finish_time;


void part_map_init(){
  for (uint64_t i=0;i<g_part_cnt;i++){
    #if (PART_TO_NODE == HASH_MODE)
      vector<uint64_t> vtmp;
      vtmp.emplace_back(i % g_node_cnt);
      vtmp.emplace_back(0);
      part_map[i] = vtmp;
    #elif (PART_TO_NODE == CONST_MODE)
      vector<uint64_t> vtmp;
      vtmp.emplace_back(i / (g_part_cnt / g_node_cnt));
      vtmp.emplace_back(0);
      part_map[i] = vtmp;
    #endif
  }
}

uint64_t get_part_node_id(uint64_t part_id){
  return part_map[part_id][0];
}

uint64_t get_part_status(uint64_t part_id){
  return part_map[part_id][1];
}

void update_part_map(uint64_t part_id, uint64_t node_id){
  //std::lock_guard<std::mutex> lock(mtx_part_map);
  part_map[part_id][0] = node_id;
}

void update_part_map_status(uint64_t part_id, uint64_t status){
  //std::lock_guard<std::mutex> lock(mtx_part_map);
  part_map[part_id][1] = status;
}

map <uint64_t, vector<uint64_t> > minipart_map;
std::mutex mtx_minipart_map;

void minipart_map_init(){
  for (uint64_t i=0; i<g_part_split_cnt; i++){
    vector<uint64_t> vtmp;
    vtmp.emplace_back(0);
    vtmp.emplace_back(0);
    minipart_map[i] = vtmp;
  }
}

uint64_t get_minipart_id(uint64_t key){
#if WORKLOAD == YCSB
  return key / g_part_cnt / (g_synth_table_size / g_part_cnt / g_part_split_cnt);
#elif WORKLOAD == TPCC
  return (key -1 - ((MIGRATION_PART+1) * g_dist_per_wh + (MIGRATION_PART+1)) * g_cust_per_dist) / (g_dist_per_wh * g_cust_per_dist / PART_SPLIT_CNT);
#endif
}

uint64_t get_minipart_node_id(uint64_t minipart_id){
  return minipart_map[minipart_id][0];
}

uint64_t get_minipart_status(uint64_t minipart_id){
  return minipart_map[minipart_id][1];
}

void update_minipart_map(uint64_t minipart_id, uint64_t node_id){
  minipart_map[minipart_id][0] = node_id;
}

void update_minipart_map_status(uint64_t minipart_id, uint64_t status){
  minipart_map[minipart_id][1] = status;
}

map <uint64_t, vector<uint64_t> > squallpart_map;

void squallpart_map_init(){
  for (uint64_t i=0; i < Squall_Part_Cnt; i++){
    vector<uint64_t> vtmp;
    vtmp.emplace_back(0);
    vtmp.emplace_back(0);
    squallpart_map[i] = vtmp;
  }
}

uint64_t get_squallpart_id(uint64_t key){
#if WORKLOAD == YCSB
  return key / g_part_cnt / (g_synth_table_size / g_part_cnt / Squall_Part_Cnt);
#elif WORKLOAD == TPCC
  return (key -1 - ((MIGRATION_PART+1) * g_dist_per_wh + (MIGRATION_PART+1)) * g_cust_per_dist) / (g_dist_per_wh * g_cust_per_dist / Squall_Part_Cnt);
  //return (key) / (g_dist_per_wh * g_cust_per_dist / Squall_Part_Cnt);
#endif
}

uint64_t get_squallpart_node_id(uint64_t squallpart_id){
  return squallpart_map[squallpart_id][0];
}

uint64_t get_squallpart_status(uint64_t squallpart_id){
  return squallpart_map[squallpart_id][1];
}

void update_squallpart_map(uint64_t squallpart_id, uint64_t node_id){
  squallpart_map[squallpart_id][0] = node_id;
}

void update_squallpart_map_status(uint64_t squallpart_id, uint64_t status){
  squallpart_map[squallpart_id][1] = status;
}

map <uint64_t, vector<uint64_t> > row_map;

void row_map_init(){
  uint64_t key = 0;
  for (uint64_t i = 0; i < g_synth_table_size / g_part_cnt; i++){
    vector<uint64_t> vtmp;
    vtmp.emplace_back(0);
    vtmp.emplace_back(0);
    row_map[key] = vtmp;
    key += g_part_cnt;
  }
}
uint64_t get_row_node_id(uint64_t key){
  return row_map[key][0];
}

uint64_t get_row_status(uint64_t key){
  return row_map[key][1];
}

void update_row_map(uint64_t key, uint64_t node_id){
  row_map[key][0] = node_id;
}

void update_row_map_status(uint64_t key, uint64_t status){
  row_map[key][1] = status;
}

void update_row_map_order(uint64_t order, uint64_t node_id){
  for (uint64_t i=0;i<order_map[Order[order]].size();i++){
    update_row_map(order_map[Order[order]][i], node_id);
  }
}

void update_row_map_status_order(uint64_t order, uint64_t status){
  for (uint64_t i=0;i<order_map[Order[order]].size();i++){
    update_row_map_status(order_map[Order[order]][i], status);
  }
}

map<uint64_t, vector<uint64_t> > order_map;

void order_map_init(){
  int a;
  for (int i=0; i<SPLIT_NODE_NUM;i++)
  {
    a = cluster[i];
    for (int j=i*ROW_PER_NODE; j<(i+1)*ROW_PER_NODE; j++){
      order_map[a].emplace_back(j*g_part_cnt);
    } 
  }

  for (uint64_t j = SPLIT_NODE_NUM * ROW_PER_NODE; j < (g_synth_table_size / g_part_cnt); j++){
    order_map[a].emplace_back(j*g_part_cnt);  
  }
}

//int Order[SPLIT_NODE_NUM]={2,3,1,0};
int Order[PART_SPLIT_CNT]={0,1,2,3};
//int Order[SPLIT_NODE_NUM]={3,0,2,1};

//int cluster[300]={2,3,0,0,3,1,3,1,0,2,2,1,3,2,0,3,3,3,1,3,3,1,0,0,0,1,2,3,3,0,1,2,0,2,0,0,2,0,2,1,2,2,1,3,1,3,0,2,2,3,3,0,3,1,3,2,0,2,2,2,0,0,2,1,2,3,2,1,1,3,2,0,2,2,3,1,2,2,3,1,3,0,2,1,0,2,3,2,2,0,1,3,1,0,2,3,3,2,2,0,0,2,0,3,3,2,3,3,2,2,2,2,2,1,2,2,1,2,0,2,0,3,2,0,3,0,3,1,2,1,0,2,2,0,2,1,3,2,3,2,2,3,1,3,0,3,0,3,2,0,1,1,3,0,2,0,1,3,2,3,0,1,3,2,1,0,2,2,2,3,1,2,3,2,0,0,2,3,0,3,3,0,2,0,3,2,2,3,2,1,1,0,3,0,3,1,2,0,2,3,2,0,0,2,0,1,3,0,2,2,3,3,3,0,2,3,2,0,1,0,3,2,0,2,1,3,1,1,2,1,3,0,2,0,2,0,2,1,3,3,0,3,2,0,2,1,1,2,2,0,2,2,1,0,3,3,0,2,2,0,2,3,3,2,1,3,3,2,1,0,2,0,2,2,3,2,0,0,3,3,2,1,2,0,0,0,2,0,0,0,0,2,1,2,3,0,0,0,3,1};

int cluster[300]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,3,3,3,3,3,3,3,3,3};

int cluster_num[PART_SPLIT_CNT];

void cluster_num_init(){
  for (int i=0;i<SPLIT_NODE_NUM;i++){
    cluster_num[cluster[i]] += ROW_PER_NODE;
  }
}

std::vector<int> Status(PART_SPLIT_CNT);

double theta = 0.20;

int detest_status; 
void update_detest_status(int status){
  detest_status = status;
}

int remus_status;
void update_remus_status(int status){
  remus_status = status;
}

int squall_status;
void update_squall_status(int status){
  squall_status = status;
}

uint64_t migrate_label;
void update_migrate_label(uint64_t status){
  migrate_label = status;
}

int synctime = SYNCTIME;