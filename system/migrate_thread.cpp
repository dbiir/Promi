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
    /*
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
    */
	if (!ISCLIENT) {
		txn_man = (YCSBTxnManager *)
		    mem_allocator.align_alloc( sizeof(YCSBTxnManager));
		new(txn_man) YCSBTxnManager();
		// txn_man = (TxnManager*) malloc(sizeof(TxnManager));
		uint64_t thd_id = get_thd_id();
		txn_man->init(thd_id, _wl);
        txn_man->h_wl = (YCSBWorkload*)_wl;
	}    
}

TxnManager * MigrateThread::get_transaction_manager(MigrationMessage * msg) {
    #if CC_ALG == CALVIN
        TxnManager* local_txn_man =
        txn_table.get_transaction_manager(get_thd_id(), msg->get_txn_id(), msg->get_batch_id());
    #else
        TxnManager * local_txn_man = txn_table.get_transaction_manager(get_thd_id(),msg->get_txn_id(),0);
    #endif
    return local_txn_man;
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
        MigrationMessage* msg = new(MigrationMessage);
        printf("begin receive message!\n");
        while(migmsg_queue.get_size()==0){
            if (simulation->is_done()) return RCOK;
        }
        //std::cout<<"the size is "<<migmsg_queue.get_size()<<endl;
        uint64_t dest = migmsg_queue.dequeue(get_thd_id(),msg);
        //std::cout<<"the size is "<<migmsg_queue.get_size()<<endl;

        assert(dest>=0);
        printf("get message111!\n");
        //if (msg->node_id_src != g_node_id) continue;
        printf("get message!\n");
        //std::cout<<"node_id_des is "<<msg->node_id_des<<endl;
        std::cout<<"message type is:"<<msg->get_rtype()<<" by migrate dequeue"<<endl;
        //std::cout<<"the size of msg is "<<msg->get_size()<<endl;
    
        //std::cout<<"node_id_des is "<<msg->node_id_des<<endl;
        RC rc;
        txn_man = get_transaction_manager(msg);
        txn_man->register_thread(this);
        txn_man->h_wl = _wl;
        switch(msg->get_rtype()){
            case SEND_MIGRATION:
                rc = process_send_migration(msg);
                assert(rc==RCOK);
                break;
            case RECV_MIGRATION:
                rc = process_recv_migration(msg);
                assert(rc==RCOK);
                break;
            case FINISH_MIGRATION:
                rc = process_finish_migration(msg);
                assert(rc==RCOK);
                break;
            default:
                printf("message->rtpye is %d\n",msg->get_rtype());
                break;
        }
    }
    return RCOK;
}

RC MigrateThread::process_send_migration(MigrationMessage* msg){
    DEBUG("SEND_MIGRATION %ld\n",msg->get_txn_id());
    printf("SEND_MIGRATION\n");
    std::cout<<&msg<<endl;
    RC rc =RCOK;
    //msg->copy_to_txn(txn_man);
    //std::cout<<"copy!"<<endl;
    txn_man->return_id = msg->return_node_id;
    txn_man->h_wl = _wl;
    /*
    for (size_t i=0;i<msg->data_size;i++){
        Access* access = new(Access);
        access->type = WR;
        access->data = &msg->data[i];
        txn_man->txn->accesses.add(access);
    }
    */
    msg->isdata = false;
    //std::cout<<&msg<<endl;
    std::cout<<"the size of msg is "<<msg->get_size()<<endl;
    //对迁移数据加锁, 将迁移数据读入msg
    idx_key_t key = 0;//起始的key，是迁移的part中第一个key，随着part_id变化而变化
	for (size_t i=0;i<msg->data_size;i++){
        //itemid_t* item = new(itemid_t);
        //std::cout<<"index addr is: "<<((YCSBWorkload*)txn_man->h_wl)->the_index<<endl;
        //item = txn_man->index_read(((YCSBWorkload*)txn_man->h_wl)->the_index,key,key_to_part(key),get_thd_id());
        
        itemid_t* item1 = (itemid_t*)mem_allocator.alloc(sizeof(itemid_t));
	    itemid_t* &item = item1;
        RC rc = ((YCSBWorkload*)_wl)->the_index->index_read(key,item,key_to_part(key),g_thread_cnt); //使用g_thread_cnt是为了访问索引的cur_leaf_per_thd数组，索引只允许工作线程和迁移线程访问，数组大小和这两个线程数量之和相同
        assert(rc==RCOK);
        //std::cout<<"OK"<<endl;
        
		//((YCSBWorkload*)_wl)->the_index->index_read(key,item,msg->part_id,get_thd_id());        
        row_t* row = ((row_t*)item->location);
        //rc = txn_man->get_lock(row,WR); fix
        row_t* row_rtn = new(row_t);
        rc = txn_man->get_row(row,access_t::WR,row_rtn);
        if (rc != RCOK){
            std::cout<<"trying to get lock..."<<endl;
        }
        while(rc != RCOK){
            rc = txn_man->get_row(row,access_t::WR,row_rtn);
        }
        //std::cout<<"key is"<<row_rtn->get_primary_key();
        //std::cout<<" "<<(string)(row_rtn->get_data())<<endl;
        msg->data.emplace_back(*row_rtn);
        //读row->data的数据
        /*fix
		char* tmp_char = row->get_data();
		string tmp_str;
		size_t k=0;
		while (tmp_char[k]!='\0'){
			tmp_str[k]=tmp_char[k];
			k++;
		}
        std::cout<<"k is "<<k<<" ";
        std::cout<<"tmp_str is "<<tmp_str<<endl;
		msg->row_data.emplace_back(tmp_str);
        std::cout<<"lock "<<key<<" ";
        std::cout<<"key is "<<msg->data[i].get_primary_key();
        std::cout<<" data is "<<msg->row_data[i]<<endl;
        key += g_part_cnt;
        */
        char* tmp_char = new char[row->tuple_size];
        memcpy(tmp_char, row->get_data(), row->tuple_size);
        string tmp_str = tmp_char;
        //std::cout<<"tuple_size is "<<row->tuple_size<<' ';
        //std::cout<<"tmp_char is "<<tmp_char[0]<<' ';
        //std::cout<<"tmp_str is "<<tmp_str<<endl;
		msg->row_data.emplace_back(tmp_str);
        std::cout<<"lock "<<key<<" ";
        //std::cout<<"key is "<<msg->data[i].get_primary_key();
        //std::cout<<" data is "<<msg->row_data[i]<<endl;
        if (KEY_TO_PART == HASH_MODE) key += g_part_cnt;
        else key ++;
	}
    msg->isdata=true;
    printf("the size of msg row_data is %ld\n",msg->row_data.size());
    std::cout<<"the size of msg is "<<msg->get_size()<<endl;

    //生成RECV_MIGRATION消息发给目标节点
    if(rc != WAIT) {//记得初始化MigrationMessage的return_node_id
        MigrationMessage * msg1 = new(MigrationMessage);
        *msg1 = *msg;
        msg1->return_node_id = g_node_id;
        msg1->rtype = RECV_MIGRATION;
        msg_queue.enqueue(get_thd_id(), msg1, msg1->node_id_des);
        free(msg);
        std::cout<<"enqueue finished!"<<endl;
    }
    return rc;
}

RC MigrateThread::process_recv_migration(MigrationMessage* msg){
    DEBUG("RECV_MIGRATION %ld\n",msg->get_txn_id());
    printf("RECV_MIGRATION\n");
    RC rc = RCOK;
    //把数据从msg里复制出来
    auto* data_ptr = mem_allocator.alloc(sizeof(row_t)*msg->data_size); //存row_t
    char* row_data_ptr = (char* )malloc((msg->data[0].tuple_size) *  (msg->data_size)); //存row_t->data
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
    printf("FINISH_MIGRATION\n");
    RC rc = RCOK;
    //msg->copy_to_txn(txn_man);
    //txn_man->h_wl = _wl;
    txn_man->release_locks(rc);
    return rc;
}