#include "migrate_thread.h"
#include "migmsg_queue.h"

#include "global.h"
#include "helper.h"
#include "manager.h"
#include "thread.h"
#include "io_thread.h"
#include "query.h"
#include "ycsb_query.h"
#include "tpcc_query.h"
#include "mem_alloc.h"
#include "transport.h"
#include "math.h"
#include "msg_thread.h"
#include "msg_queue.h"
#include "message.h"
#include "client_txn.h"
#include "work_queue.h"
#include "txn.h"
#include "ycsb.h"
#include "index_btree.h"
#include "table.h"

void MigrateThread::setup() {
    std::vector<Message*> * msgs;
	while(!simulation->is_setup_done()) {
		msgs = tport_man.recv_msg(get_thd_id());
		if (msgs == NULL) continue;
		while(!msgs->empty()) {
			Message * msg = msgs->front();
			if(msg->rtype == INIT_DONE) {
				printf("Received INIT_DONE from node %ld\n",msg->return_node_id);
				fflush(stdout);
				simulation->process_setup_msg();
			} else {
				assert(ISSERVER || ISREPLICA);
				//printf("Received Msg %d from node %ld\n",msg->rtype,msg->return_node_id);
                #if CC_ALG == CALVIN
			        if(msg->rtype == CALVIN_ACK ||(msg->rtype == CL_QRY && ISCLIENTN(msg->get_return_id())) ||
				    (msg->rtype == CL_QRY_O && ISCLIENTN(msg->get_return_id()))) {
				    work_queue.sequencer_enqueue(get_thd_id(),msg);
				    msgs->erase(msgs->begin());
				    continue;
			    }
			    if( msg->rtype == RDONE || msg->rtype == CL_QRY || msg->rtype == CL_QRY_O) {
				    assert(ISSERVERN(msg->get_return_id()));
				    work_queue.sched_enqueue(get_thd_id(),msg);
				    msgs->erase(msgs->begin());
				    continue;
			    }
                #endif
				work_queue.enqueue(get_thd_id(),msg,false);
			}
			msgs->erase(msgs->begin());
		}
		delete msgs;
	}
	if (!ISCLIENT) {
		txn_man = (YCSBTxnManager *)
		    mem_allocator.align_alloc( sizeof(YCSBTxnManager));
		new(txn_man) YCSBTxnManager();
		// txn_man = (TxnManager*) malloc(sizeof(TxnManager));
		uint64_t thd_id = get_thd_id();
		txn_man->init(thd_id, NULL);
	}    
}

RC MigrateThread::run(){
    tsetup();
    printf("Running MigrateThread %ld\n",_thd_id);

    //uint64_t ready_starttime;
    //uint64_t idle_starttime = 0;

    while(!simulation->is_done()) {
        txn_man = NULL;
        heartbeat();
        progress_stats();
        Message* msg;
        while (1){
            if (migmsg_queue.get_size()>0){
                uint64_t dest = migmsg_queue.dequeue(get_thd_id(),msg);
                if (dest>0) break;
            }
            if (simulation->is_done()) break;
        }
        if (simulation->is_done()) break;
        RC rc;
        switch(msg->get_rtype()){
            case SEND_MIGRATION:
                rc = process_send_migration((MigrationMessage*)msg);
                assert(rc==RCOK);
                break;
            case RECV_MIGRATION:
                rc = process_recv_migration((MigrationMessage*)msg);
                assert(rc==RCOK);
                break;
            case FINISH_MIGRATION:
                rc = process_finish_migration((MigrationMessage*)msg);
                assert(rc==RCOK);
                break;
            default:
                break;
        }
    }
    return RCOK;
}

RC MigrateThread::process_send_migration(MigrationMessage* msg){
    DEBUG("SEND_MIGRATION %ld\n",msg->get_txn_id());
    RC rc =RCOK;
    msg->copy_to_txn(txn_man);
    txn_man->h_wl = _wl;
    for (size_t i=0;i<msg->data_size;i++){
        Access* access = new(Access);
        access->type = WR;
        access->data = &msg->data[i];
        txn_man->txn->accesses.add(access);
    }
    //对迁移数据加锁, 将迁移数据读入msg
    idx_key_t key = 0;//起始的key，是迁移的part中第一个key，随着part_id变化而变化
	for (size_t i=0;i<msg->data_size;i++){
		itemid_t* item = new(itemid_t);
		((YCSBWorkload*)_wl)->the_index->index_read(key,item,msg->part_id,g_thread_cnt-1);
        row_t* row = ((row_t*)item->location);
        //rc = txn_man->get_lock(row,WR); fix
        row_t* row_rtn;
        rc = txn_man->get_row(row,access_t::WR,row_rtn);
        msg->data.emplace_back(*row);
        //读row->data的数据
		char* tmp_char = row->get_data();
		string tmp_str;
		size_t k=0;
		while (tmp_char[k]!='\0'){
			tmp_str[k]=tmp_char[k];
			k++;
		}
		msg->row_data.emplace_back(tmp_str);
		key += g_part_cnt;
	}
    std::cout<<"the size of msg row_data is "<<msg->row_data.size()<<endl;

    //生成RECV_MIGRATION消息发给目标节点
    if(rc != WAIT) {//记得初始化MigrationMessage的return_node_id
        msg_queue.enqueue(get_thd_id(),Message::create_message(txn_man,RECV_MIGRATION),msg->node_id_des);
    }
    return rc;
}

RC MigrateThread::process_recv_migration(MigrationMessage* msg){
    DEBUG("RECV_MIGRATION %ld\n",msg->get_txn_id());
    RC rc = RCOK;
    //把数据从msg里复制出来
    auto* data_ptr = mem_allocator.alloc(sizeof(row_t)*msg->data_size); //存row_t
    char* row_data_ptr = (char* )malloc(msg->data[0].get_tuple_size() *  msg->data_size); //存row_t->data
    uint64_t ptr = 0;
    uint64_t ptr_data = 0;
    for (size_t i=0;i<msg->data_size;i++){
        row_t* data_ = &msg->data[i];
        memcpy(data_ptr,(const void*)data_,ptr);
        ptr += sizeof(row_t);
        memcpy(row_data_ptr,(const void*)&msg->row_data,ptr_data);
        ptr_data += sizeof(data_->get_tuple_size());
        //本地生成索引
        ((YCSBWorkload*)_wl)->the_table->get_new_row(data_);
        itemid_t * m_item = (itemid_t *) mem_allocator.alloc(sizeof(itemid_t));
	    assert(m_item != NULL);
	    m_item->type = DT_row;
	    m_item->location = row_data_ptr+ptr_data-sizeof(data_->get_tuple_size());
	    m_item->valid = true;
	    uint64_t idx_key = data_->get_primary_key();
	    rc = ((YCSBWorkload*)_wl)->the_index->index_insert(idx_key, m_item, msg->part_id);
        assert(rc == RCOK);
    }
    //生成FINISH_MIGRATION消息给源节点
    if (rc==RCOK){
        msg_queue.enqueue(get_thd_id(),Message::create_message(txn_man,FINISH_MIGRATION),msg->node_id_src);
    }
    return rc;
}

RC MigrateThread::process_finish_migration(MigrationMessage* msg){
    DEBUG("FINISH_MIGRATION %ld\n",msg->get_txn_id());
    RC rc = RCOK;
    msg->copy_to_txn(txn_man);
    txn_man->h_wl = _wl;
    txn_man->release_locks(rc);
    return rc;
}