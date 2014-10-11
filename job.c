#include "job.h"
#include <stdio.h>

int init_job(job_t * job, char* chunkFile)
{
    FILE* file = fopen(chunkFile,"r");
    int ch,line_number = 0;
    int i = 0;
    char read_buffer[BUF_SIZE];
    
    /* get chunks number */
    while (fgets(read_buffer, BUF_SIZE,file)) {
        line_number++;
    }
    memset(read_buffer,0,BUF_SIZE);
    
    job.num_chunk = line_number;
    job.chunks = malloc(sizeof(chunk_t) * job.num_chunk);
    
    /* set ptr to the beginning */
    fseek(file,0,SEEK_SET);
    
    while( fgets(read_buffer,BUF_SIZE,file)) {
        job[i] = malloc(sizeof(chunk_t));
        sscanf(buf,"%d %s",&(job.chunks[i].id),jobs.chunks[i].hash);
        i++;
    }
    
    
}

int if_Finished(job_t * job)
{
    int i = 0;
    while(i < job.num_chunk) {
        if(job[i].data == NULL)
            return -1;
        i++;
    }
    return 0;
}
