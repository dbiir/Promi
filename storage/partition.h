#ifndef _PARTITION_H_
#define _PARTITION_H_

#include "global.h"
#include "message.h"
#include "helper.h"
#include "ycsb.h"
#include "row.h"
#include "thread.h"

#include "global.h"

class Partition_t{
public:
    Partition_t(uint64_t part_id, int minipart_id);
    Partition_t(uint64_t part_id, int minipart_id, uint64_t key_s, uint64_t key_e);
    RC split();
    void set_key(uint64_t key_s, uint64_t key_e);
    uint64_t get_node();
    uint64_t get_partition();

public:
    vector<Partition_t*> minipart;
    uint64_t node_id;
    uint64_t part_id;
    int minipart_id;
    uint64_t key_start, key_end;
    int part_size;
};

#endif