#include "job.h"
#include "chunk.h"
#include "sha.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>

extern bt_config_t config;

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
    
    while( fgets(read_buffer,BUF_SIZE,file)) {
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
