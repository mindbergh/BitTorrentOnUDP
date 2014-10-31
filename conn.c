#include "bt_parse.h"
#include "job.h"
#include "time.h"
#include "queue.h"
#include "conn.h"

/** @brief Initilize a downloading queue
 *  @param The pointer to the pool for initialization
 *  @return null
 */
void init_down_pool(down_pool_t* pool) {
	int i = 0 ;
	int* flags = pool->flag;
	while(i < 10) {
		flags[i++] = 0;
	}
}

/** @brief Initilize a uploading queue
 *  @param The pointer to the pool for initialization
 *  @return null
 */
void init_up_pool(up_pool_t* pool) {
	int i = 0 ;
	int* flags = pool->flag;
	while(i < 10) {
		flags[i++] = 0;
	}
}

/** @brief Initilize a downloading connection
 *  @param pointer to the connection for initialization
 *  @param Pointer to the peer which at the another side
 *  @param chunk for downloading
 *  @return null
 */
void init_down_conn(down_conn_t* conn, bt_peer_t* provider, 
	chunk_t* chunk, queue_t* get_queue) {
	conn->provider = provider;
	conn->chunks = chunk;
	conn->get_queue = get_queue;
	conn->next_pkt = 1;
}

/** @brief Initilize a uploading connection
 *  @param pointer to the connection for initialization
 *  @param Pointer to the peer which at the another side
 *  @param packet array which contains the packet to be sent
 *  @return null
 */
void init_up_conn(up_conn_t* conn, bt_peer_t* receiver,  
	data_packet_t* pkt_array) {
	conn->receiver = receiver;
	conn->pkt_array = pkt_array;
	conn->l_ack = 0;
	conn->l_available = 1;
	conn->duplicate = 0;
	conn->cwnd = INIT_CWND;
	conn->ssthresh = INIT_SSTHRESH;
}

/** @brief add a downloading connection to download pool
 *  @param pointer to download connection pool
 *  @param Pointer to the peer which at the another side
 *  @param the list of all chunks which currect connection associate
 *  @param the get requests queue
 *  @return null if pool is full, new connection if added successfully
 */
down_conn_t* en_down_pool(down_pool_t* pool,bt_peer_t* provider, 
	chunk_t* chunk, queue_t* get_queue) { 
	if( pool->num >= 10) {
		return NULL;
	}

	// find next available connection position
	int i = 0;
	while(i<10) {
		if( pool->flag[i] == 0)
			break;
		i++;
	}
	init_down_conn(&(pool->connection[i]),provider,chunk, get_queue);
	pool->flag[i] = 1;
	pool->num++;
	return pool->connection+i;
}

/** @brief add a uploading connection to upload pool
 *  @param pointer to upload connection pool
 *  @param Pointer to the peer which at the another side
 *  @param the list of all chunks which currect connection associate
 *  @param the get requests queue
 *  @return null if pool is full, new connection if added successfully
 */
up_conn_t* en_up_pool(up_pool_t* pool,bt_peer_t* receiver,  
	data_packet_t* pkt_array) { 
	if( pool->num >= 10) {
		return NULL;
	}
	// find next available connection position
	int i = 0;
	while(i<10) {
		if( pool->flag[i] == 0)
			break;
		i++;
	}
	init_up_conn(&(pool->connection[i]),receiver,pkt_array);
	pool->flag[i] = 1;
	pool->num++;
	return pool->connection+i;
}

/** @brief remove a certain connection from the upload pool
 *  @param upload pool
 *  @param the peer which connection associate with 
 *  @return null
 */
void de_up_pool(up_pool_t* pool,bt_peer_t* peer) {
	int i = 0;
	up_conn_t* conns = pool->connection;
	while( i < 10 && pool->flag[i] == 1) {
		if( conns[i].receiver->id == peer->id) {
			conns[i].receiver = NULL;
			free(conns[i].pkt_array);
			conns[i].pkt_array = NULL;
			conns[i].l_ack = 0;
			conns[i].l_available = 1;
			conns[i].duplicate = 0;
			conns[i].cwnd = INIT_CWND;
			conns[i].ssthresh = INIT_SSTHRESH;
			pool->flag[i] = 0;
			pool->num--;
			break;
		}
		i++;
	}
}

/** @brief remove a certain connection from the download pool
 *  @param download pool
 *  @param the peer which connection associate with 
 *  @return null
 */
void de_down_pool(down_pool_t* pool,bt_peer_t* peer) {
	int i = 0;
	down_conn_t* conns = pool->connection;
	while( i < 10 ) {
		if( conns[i].provider->id == peer->id && pool->flag[i] == 1) {
			if(dequeue(conns[i].get_queue) != NULL ) {
				// This should never happen!
				fprintf(stderr, "downloading connection pool error!\n");
			}
			conns[i].provider = NULL;
			conns[i].chunks = NULL;
			conns[i].get_queue = NULL;
			pool->flag[i] = 0;
			pool->num--;
			break;
		}
		i++;
	}
}

/** @brief get the pointer to a certain connection from the download pool
 *  @param download pool
 *  @param the peer which connection associate with 
 *  @return NULL if no such connection found, a pointer to the connection if is in the pool
 */
down_conn_t* get_down_conn(down_pool_t* pool, bt_peer_t* peer) {
	int i = 0; 
	down_conn_t* conns = pool->connection;
	while( i<=10 ) {
		if( conns[i].provider->id == peer->id && pool->flag[i] == 1) {
			return &conns[i];	
		}
	}
}


/** @brief get the pointer to a certain connection from the upload pool
 *  @param upload pool
 *  @param the peer which connection associate with 
 *  @return NULL if no such connection found, a pointer to the connection if is in the pool
 */
up_conn_t* get_up_conn(up_pool_t* pool, bt_peer_t* peer) {
	int i = 0; 
	up_conn_t* conns = pool->connection;
	while( i<=10 ) {
		if( conns[i].receiver->id == peer->id && pool->flag[i] == 1) {
			return &conns[i];	
		}
	}
}


/** @brief recursively send data packet from upload connection
 *  @param upload pool
 *  @param the address where pkt is going to be send to
 *  @return NULL
 */
void up_conn_recur_send(up_conn_t* conn, struct sockaddr* to) {
	while(conn->l_available - conn->l_ack <= conn->cwnd) {
		packet_sender(conn->pkt_array+conn->l_available,to);
		conn->l_available++;
	}
}

/** @brief update the data pkt when new get came
 *  @param upload pool
 *  @param new get packet
 *  @return NULL
 */
void update_up_conn(up_conn_t* conn, bt_peer_t* peer, data_packet_t* get_pkt) {
	// construct new data pkt array
	data_packet_t* data_pkt_array = DATA_pkt_array_maker(get_pkt);
	init_up_conn(conn,peer,data_pkt_array);
}




