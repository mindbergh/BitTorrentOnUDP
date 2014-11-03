#include "bt_parse.h"
#include "job.h"
#include "queue.h"
#include "conn.h"

extern bt_config_t config;
extern job_t job;

/** @brief Initilize a downloading queue
 *  @param The pointer to the pool for initialization
 *  @return null
 */
void init_down_pool(down_pool_t* pool) {
	int i = 0 ;
	int max = config.max_conn;
	pool->flag = (int*)malloc(sizeof(int)*max);
	pool->connection = (down_conn_t**)malloc(sizeof(down_conn_t*)*max);	
	int* flags = pool->flag;
	while(i < max) {
		flags[i++] = 0;
	}
}

/** @brief Initilize a uploading queue
 *  @param The pointer to the pool for initialization
 *  @return null
 */
void init_up_pool(up_pool_t* pool) {
	int i = 0 ;
	int max = config.max_conn;

	pool->flag = (int*)malloc(sizeof(int)*max);
	pool->connection = (down_conn_t*)malloc(sizeof(down_conn_t));
	int* flags = pool->flag;
	while(i < max) {
		flags[i++] = 0;
	}
<<<<<<< HEAD
	//fprintf(stderr, "%f\n", job.cwnd);
=======
	fprintf(stderr, "%f\n", job.cwnd);
>>>>>>> 55a3499114b5342d7b4ee4c8fb97ca71b1e83550
}

/** @brief Initilize a downloading connection
 *  @param pointer to the connection for initialization
 *  @param Pointer to the peer which at the another side
 *  @param chunk for downloading
 *  @return null
 */
void init_down_conn(down_conn_t** conn, bt_peer_t* provider, 
	queue_t* chunk, queue_t* get_queue) {
	(*conn) = (down_conn_t*)malloc(sizeof(down_conn_t));
	(*conn)->provider = provider;
	(*conn)->chunks = chunk;
	(*conn)->get_queue = get_queue;
	(*conn)->next_pkt = 1;
	gettimeofday(&((*conn)->last_time), NULL); // initial time
}

/** @brief Initilize a uploading connection
 *  @param pointer to the connection for initialization
 *  @param Pointer to the peer which at the another side
 *  @param packet array which contains the packet to be sent
 *  @return null
 */
void init_up_conn(up_conn_t** conn, bt_peer_t* receiver,  
	data_packet_t** pkt_array) {
	(*conn) = (up_conn_t*)malloc(sizeof(up_conn_t));
	(*conn)->receiver = receiver;
	(*conn)->pkt_array = pkt_array;
	(*conn)->l_ack = 0;
	(*conn)->l_available = 1;
	(*conn)->duplicate = 0;
	(*conn)->cwnd = INIT_CWND;
	(*conn)->ssthresh = INIT_SSTHRESH;
}

/** @brief add a downloading connection to download pool
 *  @param pointer to download connection pool
 *  @param Pointer to the peer which at the another side
 *  @param the list of all chunks which currect connection associate
 *  @param the get requests queue
 *  @return null if pool is full, new connection if added successfully
 */
down_conn_t* en_down_pool(down_pool_t* pool,bt_peer_t* provider, 
queue_t* chunk, queue_t* get_queue) { 
	if( pool->num >= config.max_conn) {
		return NULL;
	}
	// find next available connection position
	int i = 0;
	while (i<10) {
		if( pool->flag[i] == 0)
			break;
		i++;
	}
	init_down_conn(&(pool->connection[i]),provider,chunk, get_queue);
	pool->flag[i] = 1;
	pool->num++;
<<<<<<< HEAD
	//int pkt_size = ((data_packet_t*)(pool->connection[i]->get_queue->head->data))->header.packet_len;
=======
	int pkt_size = ((data_packet_t*)(pool->connection[i]->get_queue->head->data))->header.packet_len;
>>>>>>> 55a3499114b5342d7b4ee4c8fb97ca71b1e83550
	return pool->connection[i];
}

/** @brief add a uploading connection to upload pool
 *  @param pointer to upload connection pool
 *  @param Pointer to the peer which at the another side
 *  @param the list of all chunks which currect connection associate
 *  @param the get requests queue
 *  @return null if pool is full, new connection if added successfully
 */
up_conn_t* en_up_pool(up_pool_t* pool,bt_peer_t* receiver,  
	data_packet_t** pkt_array) { 
	if( pool->num >= config.max_conn) {
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
	return pool->connection[i];
}

/** @brief remove a certain connection from the upload pool
 *  @param upload pool
 *  @param the peer which connection associate with 
 *  @return null
 */
void de_up_pool(up_pool_t* pool,bt_peer_t* peer) {
	int i = 0;
	up_conn_t** conns = pool->connection;
	while( i < config.max_conn ) {
		if( pool->flag[i] == 1 && conns[i]->receiver->id == peer->id) {
			conns[i]->receiver = NULL;
			free(conns[i]->pkt_array);
			conns[i]->pkt_array = NULL;
			conns[i]->l_ack = 0;
			conns[i]->l_available = 1;
			conns[i]->duplicate = 0;
			conns[i]->cwnd = INIT_CWND;
			conns[i]->ssthresh = INIT_SSTHRESH;
			pool->flag[i] = 0;
			(pool->num)--;
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
	down_conn_t** conns = pool->connection;
	while( i < config.max_conn ) {
		if(pool->flag[i] == 1 && conns[i]->provider->id == peer->id) {
			if(dequeue(conns[i]->get_queue) != NULL ) {
				// This should never happen!
				fprintf(stderr, "downloading connection pool error!\n");
			}
			conns[i]->provider = NULL;
			conns[i]->chunks = NULL;
			conns[i]->get_queue = NULL;
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
	down_conn_t** conns = pool->connection;
	while( i<= config.max_conn) {
		if( pool->flag[i] == 1 && conns[i]->provider->id == peer->id) {
			return conns[i];	
		}
		i++;
	}
	return NULL;
}


/** @brief get the pointer to a certain connection from the upload pool
 *  @param upload pool
 *  @param the peer which connection associate with 
 *  @return NULL if no such connection found, a pointer to the connection if is in the pool
 */
up_conn_t* get_up_conn(up_pool_t* pool, bt_peer_t* peer) {
	int i = 0; 
	up_conn_t** conns = pool->connection;
	while( i<= config.max_conn) {
		if( pool->flag[i] == 1 && conns[i]->receiver->id == peer->id) {
			return conns[i];	
		}
		i++;
	}
	return NULL;
}


/** @brief recursively send data packet from upload connection
 *  @param upload pool
 *  @param the address where pkt is going to be send to
 *  @return NULL
 */
void up_conn_recur_send(up_conn_t* conn, struct sockaddr* to) {
	while(conn->l_available <= 512 && conn->l_available - conn->l_ack <= conn->cwnd) {
		fprintf(stderr, "send data:%d!!!!\n",conn->l_available);
		//print_pkt((data_packet_t*)(conn->pkt_array[conn->l_available-1]));
		packet_sender((data_packet_t*)(conn->pkt_array[conn->l_available-1]),to);
		conn->l_available++;
	}
}

/** @brief update the upload connection when new get came
 *  @param upload connection
 *	@param receiver peer
 *  @param new get packet
 *  @return NULL
 */
void update_up_conn(up_conn_t* conn, bt_peer_t* peer, data_packet_t* get_pkt) {
	// construct new data pkt array
<<<<<<< HEAD
	data_packet_t** data_pkt_array = DATA_pkt_array_maker(get_pkt);
=======
	data_packet_t* data_pkt_array = DATA_pkt_array_maker(get_pkt);
>>>>>>> 55a3499114b5342d7b4ee4c8fb97ca71b1e83550
	conn->receiver = peer;
	conn->pkt_array = data_pkt_array;
	conn->l_ack = 0;
	conn->l_available = 1;
	conn->duplicate = 0;
	conn->cwnd = INIT_CWND;
	conn->ssthresh = INIT_SSTHRESH;
}

/** @brief update the download conection when one chunk finished
 *  @param download connection
 *  @param provider
 *  @return NULL
 */
void update_down_conn( down_conn_t* conn, bt_peer_t* peer) {
	// removed finished GET request
	conn->next_pkt = 1;
}

void print_cwnd(up_conn_t *conn) {
<<<<<<< HEAD
    int elapsed;
    job.cwnd = fopen("./problem2-peer.txt", "a+");
    elapsed = get_time_diff(&(config.start_time));
    fprintf(job.cwnd, "%df%d\t%d\t%d\n",config.identity, conn->receiver->id, (int)(conn->cwnd), (int)elapsed);
    fclose(job.cwnd);
=======
    double elapsed;
    //job.cwnd = fopen("./problem2-peer.txt", "a+");
    //int elapsed = difftime(now,job.start_time);
    elapsed = get_time_diff(&(config.start_time));
    //fprintf(job.cwnd, "f%d\t%d\t%d\n", conn->receiver->id, (int)(conn->cwnd), (int)elapsed);
    //fclose(job.cwnd);
>>>>>>> 55a3499114b5342d7b4ee4c8fb97ca71b1e83550
    //fprintf(job.cwnd, "123");
}