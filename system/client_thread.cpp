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

#include <algorithm>
#include "global.h"
#include "thread.h"
#include "client_thread.h"
#include "query.h"
#include "ycsb_query.h"
#include "tpcc_query.h"
#include "client_query.h"
#include "transport.h"
#include "client_txn.h"
#include "msg_thread.h"
#include "msg_queue.h"
#include "wl.h"
#include "message.h"

void ClientThread::setup() {
	if( _thd_id == 0) {
		send_init_done_to_all_nodes();
	}
#if LOAD_METHOD == LOAD_RATE
	assert(g_load_per_server > 0);
	// send ~twice as frequently due to delays in context switching
	send_interval = (g_client_thread_cnt * BILLION) / g_load_per_server / 1.8;
	printf("Client interval: %ld\n",send_interval);
#endif

}

RC ClientThread::run() {

	tsetup();
	printf("Running ClientThread %ld\n",_thd_id);
	mrand = (myrand *) mem_allocator.alloc(sizeof(myrand));
	mrand->init(get_sys_clock());
	BaseQuery * m_query;
	uint64_t iters = 0;
	uint32_t num_txns_sent = 0;
	int txns_sent[g_servers_per_client];
	for (uint32_t i = 0; i < g_servers_per_client; ++i) txns_sent[i] = 0;

	run_starttime = get_sys_clock();
	#if MIGRATION
		bool ismigrate=false;
		uint64_t node_id_src=0, node_id_des=1;
		uint64_t part_id = 0;
	#endif
	while(!simulation->is_done()) {
		heartbeat();
		#if MIGRATION
			#if MIGRATION_ALG == DETEST
				if (!ismigrate && (get_thd_id() == 0) && ((get_sys_clock() - run_starttime) / BILLION == 150)){
				ismigrate = true;
				Message * msg = Message::create_message(SEND_MIGRATION);
				((MigrationMessage*)msg)->node_id_src = node_id_src;
				((MigrationMessage*)msg)->node_id_des = node_id_des;
				((MigrationMessage*)msg)->part_id = part_id;
				((MigrationMessage*)msg)->minipart_id = 0;
				((MigrationMessage*)msg)->rtype = SEND_MIGRATION;
				((MigrationMessage*)msg)->data_size = g_synth_table_size / g_part_cnt / PART_SPLIT_CNT;
				((MigrationMessage*)msg)->return_node_id = node_id_des;
				((MigrationMessage*)msg)->isdata = false;
				((MigrationMessage*)msg)->key_start = 0;
				std::cout<<"msg size is:"<<msg->get_size()<<endl;
				std::cout<<"begin migration!"<<endl;
				std::cout<<"Time is "<<(get_sys_clock() - run_starttime) / BILLION<<endl;
				msg_queue.enqueue(get_thd_id(),msg,node_id_src);
				continue;
				}
			#elif (MIGRATION_ALG == DETEST_SPLIT)
				if (!ismigrate && (get_thd_id() == 0) && ((get_sys_clock() - run_starttime) / BILLION == 30)){
					ismigrate = true;
					Message * msg = Message::create_message(SEND_MIGRATION);
					((MigrationMessage*)msg)->node_id_src = node_id_src;
					((MigrationMessage*)msg)->node_id_des = node_id_des;
					((MigrationMessage*)msg)->part_id = part_id;
					((MigrationMessage*)msg)->order = 0;
					((MigrationMessage*)msg)->rtype = SEND_MIGRATION;
					((MigrationMessage*)msg)->data_size = cluster_num[0];
					((MigrationMessage*)msg)->return_node_id = node_id_des;
					((MigrationMessage*)msg)->isdata = false;
					std::cout<<"msg size is:"<<msg->get_size()<<endl;
					std::cout<<"begin migration!"<<endl;
					std::cout<<"Time is "<<(get_sys_clock() - run_starttime) / BILLION<<endl;
					msg_queue.enqueue(get_thd_id(),msg,node_id_src);
					continue;
				}
			#elif MIGRATION_ALG == REMUS
			if (!ismigrate && (get_thd_id() == 0) && ((get_sys_clock() - run_starttime) / BILLION == 150)){
				ismigrate = true;
				Message * msg = Message::create_message(SEND_MIGRATION);
				((MigrationMessage*)msg)->node_id_src = node_id_src;
				((MigrationMessage*)msg)->node_id_des = node_id_des;
				((MigrationMessage*)msg)->part_id = part_id;
				((MigrationMessage*)msg)->minipart_id = -1;
				((MigrationMessage*)msg)->rtype = SEND_MIGRATION;
				((MigrationMessage*)msg)->data_size = g_synth_table_size / g_part_cnt;
				((MigrationMessage*)msg)->return_node_id = node_id_des;
				((MigrationMessage*)msg)->isdata = false;
				std::cout<<"msg size is:"<<msg->get_size()<<endl;
				std::cout<<"begin migration!"<<endl;
				std::cout<<"Time is "<<(get_sys_clock() - run_starttime) / BILLION<<endl;
				msg_queue.enqueue(get_thd_id(),msg,node_id_src);
				continue;
			}
			#endif
		#endif
#if SERVER_GENERATE_QUERIES
		break;
#endif
		//uint32_t next_node = iters++ % g_node_cnt;
		progress_stats();
		int32_t inf_cnt;
	//the next_node request producing is not suitable, so we change it.
	/*
	#if CC_ALG == BOCC || CC_ALG == FOCC || ONE_NODE_RECIEVE == 1
		uint32_t next_node = 0;
		uint32_t next_node_id = next_node;
	#elif (MIGRATION_ALG == REMUS)
		uint32_t next_node;
		uint32_t next_node_id;
		if (remus_status == 0 || remus_status == 1) {
			next_node = 0;
			next_node_id = next_node + g_server_start_node;
		}
		else if (remus_status == 2) {
			next_node = 1;
			next_node_id = next_node + g_server_start_node;
		}
	#else
		uint32_t next_node = (((iters++) * g_client_thread_cnt) + _thd_id )% g_servers_per_client;
		uint32_t next_node_id = next_node + g_server_start_node;
	#endif
	*/
		uint64_t partition_id = mrand->next() % g_part_cnt;
		uint32_t next_node = GET_NODE_ID(partition_id);
		uint32_t next_node_id = next_node;
		#if MIGRATION
			#if SINGLE_PART_0
				partition_id = 0;//只跑分区0的事务
				next_node = GET_NODE_ID(partition_id);
				next_node_id = next_node;
			#elif SINGLE_PART_012
				partition_id = mrand->next() % 3;
				next_node = GET_NODE_ID(partition_id);
				next_node_id = next_node;
			#elif SINGLE_PART_0124
				partition_id = mrand->next() % 4;
				switch (partition_id){
					case 0:
						partition_id = 0;
						break;
					case 1:
						partition_id = 1;
						break;
					case 2:
						partition_id = 2;
						break;
					case 3:
						partition_id = 4;
						break;	
				}
			#elif SINGLE_PART_CONSOLIDATION
				partition_id = mrand->next() % 2; //只生成01分区的事务
				next_node = GET_NODE_ID(partition_id);
				next_node_id = next_node;
			#else

			#endif
				next_node = GET_NODE_ID(partition_id);
				next_node_id = next_node;
		#endif
		// uint32_t next_node_id = next_node + g_server_start_node;
		// Just in case...
		if (iters == UINT64_MAX)
			iters = 0;
#if LOAD_METHOD == LOAD_MAX
	#if WORKLOAD != DA
		if ((inf_cnt = client_man.inc_inflight(next_node)) < 0)
			continue;
	#endif

		#if MIGRATION
			if (partition_id == 0){
				m_query = client_query_queue.get_next_query_partition(0, partition_id,_thd_id);
			} else {
				m_query = client_query_queue.get_next_query_partition(next_node, partition_id,_thd_id);
			}
		#else
			m_query = client_query_queue.get_next_query(next_node,_thd_id);
		#endif

		#if MIGRATION
			if (partition_id != key_to_part(((YCSBQuery*)m_query)->requests[0]->key))
				std::cout<<"wrong query ";
		#endif
		//根据分区迁移状态决定发到哪个节点
		#if MIGRATION  //根据分区迁移状态决定发到哪个节点
			#if MIGRATION_ALG == REMUS
				next_node = GET_NODE_ID(partition_id);
				if (next_node == 1 && next_node != next_node_id) std::cout<<"to ";
			#elif MIGRATION_ALG == DETEST
			if (partition_id == 0){
				int node0,node1;
				node0 = 0;
				node1 = 0;
				for (uint64_t i=0; i < ((YCSBQuery*)m_query)->requests.size(); i++){
					if (get_minipart_node_id(get_minipart_id(((YCSBQuery*)m_query)->requests[i]->key)) == 0) node0++;
					else node1++;
				}
				if (node1 < 1) next_node = 0;
				else next_node = 1;
				//next_node = get_minipart_node_id(mrand->next() % g_part_split_cnt);
				if (next_node == 1 && next_node != next_node_id) std::cout<<"to ";
				//else {
					//if (node0!=0 && node0!=5) std::cout<<"node0 "<<node0<<' '<<node1<<' ';
				//}
			}
			else {
				next_node = GET_NODE_ID(partition_id);
				if (next_node == 1 && next_node != next_node_id) std::cout<<"to ";	
			}
			#elif MIGRATION_ALG == DETEST_SPLIT
				next_node = get_row_node_id(mrand->next() % g_synth_table_size / g_part_cnt);
				if (next_node == 1 && next_node != next_node_id) std::cout<<"to ";
			#endif
			next_node_id = next_node;
		#endif
		if(last_send_time > 0) {
			INC_STATS(get_thd_id(),cl_send_intv,get_sys_clock() - last_send_time);
		}
		last_send_time = get_sys_clock();
		simulation->last_da_query_time = get_sys_clock();
#elif LOAD_METHOD == LOAD_RATE
		if ((inf_cnt = client_man.inc_inflight(next_node)) < 0)
			continue;
		uint64_t gate_time;
		while((gate_time = get_sys_clock()) - last_send_time < send_interval) { }
		if(last_send_time > 0) {
			INC_STATS(get_thd_id(),cl_send_intv,gate_time - last_send_time);
		}
		last_send_time = gate_time;
		m_query = client_query_queue.get_next_query(next_node,_thd_id);
#else
		assert(false);
#endif
		assert(m_query);

		DEBUG("Client: thread %lu sending query to node: %u, %d, %f\n",
				_thd_id, next_node_id,inf_cnt,simulation->seconds_from_start(get_sys_clock()));
		//std::cout<<"remus status is "<<remus_status<<" ";

		//printf("Client: thread %lu sending query to node: %u, %d, %f\n",
		//		_thd_id, next_node_id,inf_cnt,simulation->seconds_from_start(get_sys_clock()));
#if ONE_NODE_RECIEVE == 1 && defined(NO_REMOTE) && LESS_DIS_NUM == 10
		Message * msg = Message::create_message((BaseQuery*)m_query,CL_QRY_O);
#else
		Message * msg = Message::create_message((BaseQuery*)m_query,CL_QRY);
#endif
		((ClientQueryMessage*)msg)->client_startts = get_sys_clock();
		msg_queue.enqueue(get_thd_id(),msg,next_node_id);
		num_txns_sent++;
		txns_sent[next_node]++;
		query_to_part[partition_id] ++;
		vector <uint64_t> cokeys;
		/*
		for (uint64_t i = 0; i < g_req_per_query; i++){
			query_to_row[((YCSBQuery*)m_query)->requests[i]->key]++;
			if (key_to_part(((YCSBQuery*)m_query)->requests[i]->key) == 0){
				cokeys.emplace_back(((YCSBQuery*)m_query)->requests[i]->key);
				for (uint64_t j = 0 ; j < cokeys.size()-1; j++){
					//std::cout<<cokeys[j]<<' '<<((YCSBQuery*)m_query)->requests[i]->key<<endl;
					edge_index.emplace_back(cokeys[j], ((YCSBQuery*)m_query)->requests[i]->key);
				}
			}
		}
		*/
		INC_STATS(get_thd_id(),txn_sent[(get_sys_clock() - g_starttime) / BILLION],1);
		INC_STATS(get_thd_id(),txn_sent_cnt,1);
		/*
		#if MIGRATION
			if (g_migrate_flag && get_sys_clock() -   )
		#endif
		*/
		#if WORKLOAD==DA
			delete m_query;
		#endif
	}
	#if (MIGRATION_ALG == REMUS)
	std::cout<<"remus status is "<<remus_status<<" Time is:"<< get_sys_clock()<<endl;
	#endif
	for (uint64_t l = 0; l < g_servers_per_client; ++l)
		printf("Txns sent to node %lu: %d\n", l+g_server_start_node, txns_sent[l]);

	//SET_STATS(get_thd_id(), total_runtime, get_sys_clock() - simulation->run_starttime);

	printf("FINISH %ld:%ld\n",_node_id,_thd_id);
	fflush(stdout);
	return FINISH;
}
