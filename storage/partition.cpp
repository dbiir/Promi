#include "global.h"
#include "table.h"
#include "catalog.h"
#include "row.h"
#include "txn.h"
#include "partition.h"


Partition_t::Partition_t(uint64_t part_id, int minipart_id){
    part_id = part_id;
    minipart_id = minipart_id;
    if (minipart_id == -1){
        part_size = g_synth_table_size / g_part_cnt;
        key_start = part_id;
        key_end = g_synth_table_size - (g_part_cnt - part_id);
    }
    else{
        part_size = g_synth_table_size / g_part_cnt / g_part_split_cnt;        }
    }

Partition_t::Partition_t(uint64_t part_id, int minipart_id, uint64_t key_s, uint64_t key_e){
    part_id = part_id;
    minipart_id = minipart_id;
    if (minipart_id == -1){
        part_size = g_synth_table_size / g_part_cnt;
    }
    else{
        part_size = g_synth_table_size / g_part_cnt / g_part_split_cnt;
    }
    key_start = key_s;
    key_end = key_e;
}

void Partition_t::set_key(uint64_t key_s, uint64_t key_e){
    key_start = key_s;
    key_end = key_e;
}

RC Partition_t::split(){
    RC rc = RCOK;
    uint64_t minipart_size = g_synth_table_size / g_part_cnt / g_part_split_cnt;
    //uint64_t key = part_id;
    //int cnt = 0;
    minipart.clear();
    for (UInt32 i=0; i<g_part_split_cnt; i++){
        uint64_t key_s,key_e;
        key_s = minipart_size * i;
        key_e = key_s + minipart_size - g_part_cnt;
        Partition_t part_tmp = Partition_t(part_id, i, key_s, key_e);
        minipart.emplace_back(&part_tmp);
    }
    return rc;
}

uint64_t Partition_t::get_node(){return node_id;}
uint64_t Partition_t::get_partition(){return part_id;}
