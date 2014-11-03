#ifndef _JOB_H
#define _JOB_H

#include "sha.h"
#include "bt_parse.h"
#include "queue.h"
#include "chunk.h"
#include "sha.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h> 
#include <assert.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include "timer.h"
#include "spiffy.h"

#define PACKETLEN       1500
#define HEADERLEN       16
#define DATALEN         PACKETLEN - HEADERLEN
#define MAX_CHUNK		74   //Max number of chunk in a single whohas pkt
#define BUF_SIZE        60
#define PKT_WHOHAS 		0
#define PKT_IHAVE		1
#define PKT_GET			2
#define PKT_DATA		3
#define PKT_ACK 		4
#define PKT_DENIED		5      
#define CHUNK_SIZE      (1 << 19)  //size of a single chunk in Bytes
#define VERBOSE         0


typedef struct chunk_s {
	int id;
	uint8_t hash[SHA1_HASH_SIZE];
	char *data;
    int cur_size;
	int num_p;
	bt_peer_t *pvd; /* providers */
} chunk_t;
 
// num_chunk * 512 * 1024 = file_size;max num_chunk = 4095
// largest file supports is 2GB - 512KB
typedef struct job_s {
    int num_chunk;   
    int num_need;
    int num_living;
    chunk_t* chunks;
    short living_flags;
    char get_chunk_file[BT_FILENAME_LEN];
} job_t;

typedef struct header_s {
    short magicnum;
    char version;
    char packet_type;
    short header_len;
    short packet_len; 
    u_int seq_num;
    u_int ack_num;
} header_t;  

typedef struct data_packet {
    header_t header;
    char data[DATALEN];
} data_packet_t;

int init_job(char* chunkFile, char* output_file);
int is_job_finished();
int IfIHave(uint8_t *hash_start);
int packet_parser(char* buf);
void send_WhoHas(data_packet_t* pkt);
void packet_sender(data_packet_t* pkt, struct sockaddr* to);
queue_t *WhoHas_maker(void);
data_packet_t *IHave_maker(data_packet_t *whohas_pkt);
int match_need(uint8_t *hash);
queue_t* GET_maker(data_packet_t *pkt,bt_peer_t* peer, queue_t* chunk_queue);
data_packet_t* ACK_maker(int ack, data_packet_t* pkt);
data_packet_t* DENIED_maker();
void whohas_data_maker(int num_chunk, chunk_t *chunks, char* data);
void flood_WhoHas();
data_packet_t** DATA_pkt_array_maker(data_packet_t* pkt);
data_packet_t *packet_maker(int type, short pkg_len, u_int seq, u_int ack, char *data);
void store_data(chunk_t* chunk, data_packet_t* pkt);
void cat_chunks();
int is_chunk_finished(chunk_t* chunk);
void packet_free(data_packet_t *pkg);
void hostToNet(data_packet_t* pkt);
void netToHost(data_packet_t* pkt);
void print_pkt(data_packet_t* pkt);
void print_hash(uint8_t *hash);
void hostToNet(data_packet_t* pkt);
void netToHost(data_packet_t* pkt);
void clear_job();


#endif
