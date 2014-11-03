#ifndef _CONN_H
#define _CONN_H

#include "bt_parse.h"
#include "queue.h"
#include "chunk.h"
#include "sha.h"
#include "job.h"
#include "timer.h"

#define INIT_CWND     8   // initial window size   
#define INIT_SSTHRESH 64  // initial thresh size

typedef struct down_conn_s {
	bt_peer_t* provider;
	queue_t* chunks;
	queue_t* get_queue;
	struct timeval last_time;
	int next_pkt; // next expected pkt number
}down_conn_t;


typedef struct up_conn_s {
	bt_peer_t* receiver;
	data_packet_t** pkt_array;
	int l_ack;
	int l_available;
	int duplicate;
	float cwnd;
	int ssthresh;
}up_conn_t;

typedef struct down_pool_s {
	down_conn_t** connection;
	int* flag;
	int num;
}down_pool_t;

typedef struct up_pool_s {
	up_conn_t** connection;
	int* flag;
	int num;	
}up_pool_t;

void init_down_pool(down_pool_t* pool);
void init_up_pool(up_pool_t* pool);
void init_down_conn(down_conn_t** conn, bt_peer_t* provider, 
	queue_t* chunk, queue_t* get_queue);
void init_up_conn(up_conn_t** conn, bt_peer_t* receiver,  
	data_packet_t** pkt_array);
down_conn_t* en_down_pool(down_pool_t* pool,bt_peer_t* provider, 
	queue_t* chunk, queue_t* get_queue);
up_conn_t* en_up_pool(up_pool_t* pool,bt_peer_t* receiver,  
	data_packet_t** pkt_array);
void de_down_pool(down_pool_t* pool,bt_peer_t* peer);
void de_up_pool(up_pool_t* pool,bt_peer_t* peer);
down_conn_t* get_down_conn(down_pool_t* pool, bt_peer_t* peer);
up_conn_t* get_up_conn(up_pool_t* pool, bt_peer_t* peer);
void up_conn_recur_send(up_conn_t* conn, struct sockaddr* to);
void update_down_conn( down_conn_t* conn, bt_peer_t* peer);
void update_up_conn(up_conn_t* conn, bt_peer_t* peer, data_packet_t* get_pkt);
void print_cwnd(up_conn_t *conn);

#endif


