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
#include "spiffy.h"


#define PACKETLEN       1500
#define HEADERLEN       16
#define DATALEN         PACKETLEN - HEADERLEN
#define MAX_CHUNK		74
#define BUF_SIZE        60

#define PKT_WHOHAS 		0
#define PKT_IHAVE		1
#define PKT_GET			2
#define PKT_DATA		3
#define PKT_ACK 		4
#define PKT_DENIED		5



typedef struct chunk_s {
	int id;
	uint8_t hash[SHA1_HASH_SIZE];
	char *data;
	int num_p;
	bt_peer_t *pvd; /* providers */
} chunk_t;
 
typedef struct job_s {
    int num_chunk;
    int num_need;
    chunk_t* chunks;
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

int init_job(char* chunkFile);
int isFinished();
int IfIHave(uint8_t *hash_start);
int packet_parser(char* buf);
void Send_WhoHas(data_packet_t* pkt);
void packet_sender(data_packet_t* pkt, struct sockaddr * to);
queue_t *WhoHas_maker(void);
data_packet_t *IHave_maker(data_packet_t *whohas_pkt);
int match_need(uint8_t *hash);
queue_t* GET_maker(data_packet_t *pkt, bt_peer_t* provider);
void whohas_data_maker(int num_chunk, chunk_t *chunks, char* data);
data_packet_t *packet_maker(int type, short pkg_len, u_int seq, u_int ack, char *data);
void packet_free(data_packet_t *pkg);
void print_pkt(data_packet_t* pkt);
void print_hash(uint8_t *hash);


#endif
