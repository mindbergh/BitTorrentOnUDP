#ifndef _JOB_H
#define _JOB_H

typedef struct job_s {
	int num_chunk;
	chunk_t* chunks;
} job_t;

int init_job(job_t * ob, char* chunkFile);
int if_Finished(job_t * job);
void Send_WhoHas(data_packet_t*);
void packet_sender(data_packet_t* data, struct sockaddr * to);


#endif
