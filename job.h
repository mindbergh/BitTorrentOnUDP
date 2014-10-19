#ifndef _JOB_H
#define _JOB_H

#include "sha.h"
#include "bt_parse.h"
#include "queue.h"



#define PACKETLEN 		1500
#define HEADERLEN      16
#define DATALEN        PACKETLEN - HEADERLEN
#define MAX_CHUNK		74

#define PKT_WHOHAS 		0
#define PKT_IHAVE		1
#define PKT_GET			2
#define PKT_DATA		3
#define PKT_ACK 		4
#define PKT_DENIED		5

typedef struct job_s {
	int num_chunk;
	chunk_t* chunks;
} job_t;

typedef struct chunk_s {
	int id;
	uint8_t hash[SHA1_HASH_SIZE];
	char *data;
	int num_p;
	bt_peer_t *pvd; /* providers */
} chunk_t;
 

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

int init_job(job_t * ob, char* chunkFile);
int if_Finished(job_t * job);
void Send_WhoHas(data_packet_t* pkt);
void packet_sender(data_packet_t* pkt, struct sockaddr * to);
queue_t *WhoHas_maker(void);
char *whohas_data_maker(int num_chunk, chunk_t *chunks);
data_packet_t *packet_maker(int type, short pkg_len, u_int seq, u_int ack, char *data);
void packet_free(data_packet_t *pkg);


#endif
