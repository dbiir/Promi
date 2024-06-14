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
	BaseQuery * m_query;
	uint64_t iters = 0;
	uint32_t num_txns_sent = 0;
	int txns_sent[g_servers_per_client];
	for (uint32_t i = 0; i < g_servers_per_client; ++i) txns_sent[i] = 0;

	bool ismigrate=false;//default
	assert(!ismigrate);
	
	run_starttime = get_sys_clock();
	while(!simulation->is_done()) {
		heartbeat();
		#if SERVER_GENERATE_QUERIES
			break;
		#endif

		#if MIGRATION
			//init migration msg
			uint64_t node_id_src_ = 1;
			uint64_t node_id_des_ = 0;
			uint64_t part_id_ = 1;
			uint64_t minipart_id_ = 0;
			bool islast_ = true;
			std::vector<uint64_t> mig_order_;
			for (uint i=0; i<g_part_split_cnt; i++){
				mig_order_.emplace_back(i);
			}


			//msg param
			#if MIGRATION_ALG == DETEST
				uint64_t data_size_ = g_synth_table_size / g_part_cnt / g_part_split_cnt;
				uint64_t key_start_ = minipart_to_key_start(part_id_, minipart_id_);
				uint64_t key_end_ = minipart_to_key_end(part_id_, minipart_id_);
				
				assert((key_end_ - key_start_) / g_part_cnt + 1 == data_size_);
			
				islast_ = false;
			#endif

			//migrate at START_MIG
			if (!ismigrate && (get_thd_id() == 0) && ((get_sys_clock() - run_starttime) / BILLION  >= START_MIG)){
				ismigrate = true;
				Message * msg = Message::create_message(SEND_MIGRATION);
				((MigrationMessage*)msg)->node_id_src = node_id_src_;
				((MigrationMessage*)msg)->node_id_des = node_id_des_;
				((MigrationMessage*)msg)->part_id = part_id_;
				((MigrationMessage*)msg)->minipart_id = minipart_id_;
				((MigrationMessage*)msg)->rtype = SEND_MIGRATION;
				((MigrationMessage*)msg)->data_size = data_size_;
				((MigrationMessage*)msg)->return_node_id = node_id_des_;
				((MigrationMessage*)msg)->isdata = islast_;
				((MigrationMessage*)msg)->key_start = key_start_;			
				((MigrationMessage*)msg)->key_end = key_end_;	
				((MigrationMessage*)msg)->order = 0;	
				for (uint i=0; i<g_part_split_cnt; i++){
					((MigrationMessage*)msg)->mig_order.emplace_back(mig_order_[i]);
				}					

				//debug
				for (uint i=0; i< ((MigrationMessage*)msg)->mig_order.size(); i++) std::cout<<((MigrationMessage*)msg)->mig_order[i]<<' ';

				msg_queue.enqueue(get_thd_id(), msg, ((MigrationMessage*)msg)->node_id_src);

				std::cout<<"Begin migration! "<<"part_id "<<part_id_<<" minipart_id "<<minipart_id_<<endl;
				std::cout<<"Time is "<<(get_sys_clock() - run_starttime) / BILLION<<endl;				
			}

		#endif


		//uint32_t next_node = iters++ % g_node_cnt;
		progress_stats();
		int32_t inf_cnt;
		#if CC_ALG == BOCC || CC_ALG == FOCC || ONE_NODE_RECIEVE == 1
			uint32_t next_node = 0;
			uint32_t next_node_id = next_node;
		#else
			uint32_t next_node = (((iters++) * g_client_thread_cnt) + _thd_id )% g_servers_per_client;
			uint32_t next_node_id = next_node + g_server_start_node;
		#endif
		// uint32_t next_node_id = next_node + g_server_start_node;
		// Just in case...
		if (iters == UINT64_MAX)
			iters = 0;

		#if MIGRATION
			//determine the partition first
			uint64_t partition_id = (iters) % g_part_cnt;
			next_node = get_part_node_id(partition_id);
			next_node_id = next_node + g_server_start_node;
			assert(next_node_id < g_node_cnt);

			if (iters == UINT64_MAX)
				iters = 0;		

			if ((inf_cnt = client_man.inc_inflight(next_node)) < 0) {
				//std::cout<<next_node<<' ';
				//std::cout<<g_node_inflight_max[next_node]<<" ";
				continue;
			}
				
			if (partition_id % g_node_cnt == next_node) //not migrated 
				m_query = client_query_queue.get_next_query_partition(next_node,partition_id, _thd_id);
			else //migrated
				m_query = client_query_queue.get_next_query_partition(partition_id % g_node_cnt,partition_id, _thd_id);
				

			if(last_send_time > 0) {
				INC_STATS(get_thd_id(),cl_send_intv,get_sys_clock() - last_send_time);
			}
			last_send_time = get_sys_clock();
			simulation->last_da_query_time = get_sys_clock();			
		#else
			#if LOAD_METHOD == LOAD_MAX
				#if WORKLOAD != DA
					if ((inf_cnt = client_man.inc_inflight(next_node)) < 0)
						continue;
				#endif
					m_query = client_query_queue.get_next_query(next_node,_thd_id);
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
		#endif


		assert(m_query);

		DEBUG("Client: thread %lu sending query to node: %u, %d, %f\n",
				_thd_id, next_node_id,inf_cnt,simulation->seconds_from_start(get_sys_clock()));
#if ONE_NODE_RECIEVE == 1 && defined(NO_REMOTE) && LESS_DIS_NUM == 10
		Message * msg = Message::create_message((BaseQuery*)m_query,CL_QRY_O);
#else
		Message * msg = Message::create_message((BaseQuery*)m_query,CL_QRY);
#endif
		((ClientQueryMessage*)msg)->client_startts = get_sys_clock();
		msg_queue.enqueue(get_thd_id(),msg,next_node_id);
		num_txns_sent++;
		txns_sent[next_node]++;
		INC_STATS(get_thd_id(),txn_sent_cnt,1);

		#if MIGRATION
			query_to_part[partition_id] ++;
			if (partition_id == part_id_){
				for (uint i=0; i<((YCSBQuery*)m_query)->requests.size(); i++){
					uint64_t minipart_ = get_minipart_id(((YCSBQuery*)m_query)->requests[i]->key);
					query_to_minipart[minipart_]++;
				}
			}
		#endif

		#if WORKLOAD==DA
			delete m_query;
		#endif
	}

	for (uint64_t l = 0; l < g_servers_per_client; ++l)
		printf("Txns sent to node %lu: %d\n", l+g_server_start_node, txns_sent[l]);

	//SET_STATS(get_thd_id(), total_runtime, get_sys_clock() - simulation->run_starttime);

	printf("FINISH %ld:%ld\n",_node_id,_thd_id);
	fflush(stdout);
	return FINISH;
}
