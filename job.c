#include "job.h"
#include "chunk.h"
#include "sha.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>


extern bt_config_t config;
extern job_t job;

int init_job(job_t * job, char* chunkFile) {
    FILE* file = fopen(chunkFile,"r");
    int ch,line_number = 0;
    int i = 0;
    char read_buffer[BUF_SIZE];
    char hash_buffer[SHA1_HASH_SIZE*2];
    
    /* get chunks number */
    while (fgets(read_buffer, BUF_SIZE,file)) {
        line_number++;
    }
    memset(read_buffer,0,BUF_SIZE);
    
    job->num_chunk = line_number;
    job->chunks = malloc(sizeof(chunk_t) * job->num_chunk);
    
    /* set ptr to the beginning */
    fseek(file,0,SEEK_SET);
    
    while (fgets(read_buffer,BUF_SIZE,file)) {
        job->chunks[i] = malloc(sizeof(chunk_t));
        sscanf(buf,"%d %s",&(job->chunks[i].id),hash_buffer);
        
        /* convert ascii to binary hash code */
        hex2binary(hash_buffer,SHA1_HASH_SIZE*2,job->chunks[i].hash);
        
        memset(read_buffer,0,BUF_SIZE);
        memset(hash_buffer,0,SHA1_HASH_SIZE*2);
        i++;
    }    
}

int if_Finished(job_t * job) {
    int i = 0;
    while(i < job->num_chunk) {
        if(job[i]->data == NULL)
            return -1;
        i++;
    }
    return 0;
}

/** Generate WhoHas package
 *
 */
queue_t *WhoHas_maker() {
    // to do parse getchunkfile
    queue_t *q = queue_init();
    data_packet_t *pkg;
    char *data;
    short pkg_len;
    int i, j, n, m;

    if (job->num_chunk > MAX_CHUNK) {
        n = job->num_chunk / MAX_CHUNK; /* multiple of 74 */
        for (i = 0; i < n; i++) {
            pkg_len = 16 + 4 + MAX_CHUNK * SHA1_HASH_SIZE;
            data = whohas_data_maker(MAX_CHUNK, job->chunks + i * MAX_CHUNK);
            pkg = packet_maker(PKG_WHOHAS, pkg_len, 0, 0, data);
            enqueue(q, (void *)pkg);
        }
        m = job->num_chunk - n * MAX_CHUNK; /* number of chunks can fit into one pkg */
        pkg_len = 16 + 4 + m * SHA1_HASH_SIZE;
        data = whohas_data_maker(m, job->chunks + n * MAX_CHUNK);
        pkg = packet_maker(PKG_WHOHAS, pkg_len, 0, 0, data);
        enqueue(q, (void *)pkg);
    } else {
        pkg_len = 16 + 4 + job->num_chunk * SHA1_HASH_SIZE;
        data = whohas_data_maker(job->num_chunk, job->chunks);
        pkg = packet_maker(PKG_WHOHAS, pkg_len, 0, 0, data);
        enqueue(q, (void *)pkg); 
    }
    return q;
}

char *whohas_data_maker(int num_chunk, chunk_t *chunks) {
    char *data;
    short pkg_len;
    int i;
    
    /* Header:16 + num,padding:4 + hashs:20*num of chunk */
    pkg_len = 16 + 4 + num_chunk * SHA1_HASH_SIZE; 
    data = (char *)malloc(pkg_len - 16);
    data[0] = num_chunk; /* Number of chunks */
    memset(data[1], 0, 3); /* padding 3 bytes */
    data += 4;             /* shift 4 to fill in chunk hashs */
    for (i = 0; i < num_chunk; i++) {
        /* Chunk Hash 20 bytes */
        memcpy(data[i], chunks[i].hash, SHA1_HASH_SIZE);
    }
    return data;
}



data_packet_t *packet_maker(int type, short pkg_len, u_int seq, u_int ack, char *data) {
    data_packet_t *pkg = (data_packet_t *)malloc(sizeof(data_packet_t));
    pkg->header.magicnum = 15441; /* Magic number */
    pkg->header.version = 1;      /* Version number */
    pkg->header.packet_type = type; /* Packet Type */
    pkg->header.header_len = 16;    /* Header length is always 16 */
    pkg->header.packet_len = pkg_len;
    pkg->header.seq_num = seq;
    pkg->header.ack_num = ack;
    pkg->data = data;
    return pkg;
}

void packet_free(data_packet_t *pkg) {
    free(pkg->data);
    free(pkg);
}


void Send_WhoHas(data_packet_t* data) {
    
    bt_peer_s* peer = config.peers;
    while(peer != NULL) {
        packet_sender(data,(struct sockaddr *)peer->addr);
        peer = peer.next;
    }
}

void packet_sender(data_packet_t* data, struct sockaddr * to) {
    spiffy_sendto(config->myport, data, sizeof(data_packet_t), 0, to, sizeof(*to));
}
