/*
 * peer.c
 *
 * Authors: Ming Fang <mingf@andrew.cmu.edu>,
 *          Yao Zhou <>
 *          
 * Class: 15-441 (Fall 2014)
 *
 * Skeleton for 15-441 Project 2.
 *
 */

#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "debug.h"
#include "spiffy.h"
#include "bt_parse.h"
#include "input_buffer.h"
#include "queue.h"
#include "job.h"    
#include "chunk.h"

/* Function Prototypes */
void peer_run(bt_config_t *config);
void freeJob(job_t* job);
void init_hasChunk(char* has_chunk_file);

/* Global variables */
job_t job;
bt_config_t config;
queue_t* hasChunk;


int main(int argc, char **argv) {

    bt_init(&config, argc, argv);

    DPRINTF(DEBUG_INIT, "peer.c main beginning\n");

#ifdef TESTING
    config.identity = 1; // your group number here
    strcpy(config.chunk_file, "chunkfile");
    strcpy(config.has_chunk_file, "haschunks");
#endif

    bt_parse_command_line(&config);

#ifdef DEBUG
    if (debug & DEBUG_INIT) {
        bt_dump_config(&config);
    }
#endif

    peer_run(&config);
    return 0;
}


void process_inbound_udp(int sock) {
    int packet_type = -1;
    struct sockaddr_in from;
    socklen_t fromlen;
    char buf[PACKETLEN];

    fromlen = sizeof(from);
    spiffy_recvfrom(sock, buf, PACKETLEN, 0, (struct sockaddr *) &from, &fromlen);
    print_pkt((data_packet_t *)buf);
    // call packet_parser
    packet_type = packet_parser(buf);
    // switch on packet type
    switch(packet_type) {
        // case WhoHas
        case PKT_WHOHAS: {
            // Construct I have response pkt
            data_packet_t* pkt = IHave_maker((data_packet_t*)buf);
            // Send it back
            packet_sender(pkt, (struct sockaddr *) &from);
            packet_free(pkt);
            break;
        }

        case PKT_IHAVE: {
        }
        case PKT_GET: {
            break;
        }

        case PKT_DATA: {
            break;
        }

        case PKT_ACK: {
            break;
        }
        
        case PKT_DENIED: {
            break;
        }

        default: {
            // Invalid packet
            fprintf(stderr,"Invalid Packet:%d!\n", packet_type);
            break;
        }
    }

    printf("PROCESS_INBOUND_UDP SKELETON -- replace!\n"
           "Incoming message from %s:%d\n%s\n\n",
            inet_ntoa(from.sin_addr),
            ntohs(from.sin_port),
            buf);
}

void process_get(char *chunkfile, char *outputfile) {
    printf("PROCESS GET SKELETON CODE CALLED.  Fill me in!  (%s, %s)\n",
    chunkfile, outputfile);
    /* Create a Job */
    init_job(chunkfile);

    /* call whohasmaker */
    queue_t* whoHasQueue = WhoHas_maker();
    /* send out all whohas packets */
    data_packet_t* cur_pkt = NULL;
    while((cur_pkt = (data_packet_t *)dequeue(whoHasQueue)) != NULL) {
        //fprintf(stderr, "here\n");
        Send_WhoHas(cur_pkt);
        packet_free(cur_pkt);
    }
    
    /* free current job content */
    //freeJob(job);
    
}

void handle_user_input(char *line, void *cbdata) {
    char chunkf[128], outf[128];

    bzero(chunkf, sizeof(chunkf));
    bzero(outf, sizeof(outf));

    if (sscanf(line, "GET %120s %120s", chunkf, outf)) {
        if (strlen(outf) > 0) {
            process_get(chunkf, outf);
        }
    }
}


void peer_run() {
    int sock;
    struct sockaddr_in myaddr;
    fd_set readfds;
    struct user_iobuf *userbuf;

    if ((userbuf = create_userbuf()) == NULL) {
        perror("peer_run could not allocate userbuf");
        exit(-1);
    }

    if ((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP)) == -1) {
        perror("peer_run could not create socket");
        exit(-1);
    }

    bzero(&myaddr, sizeof(myaddr));
    myaddr.sin_family = AF_INET;
    //myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    inet_aton("127.0.0.1", (struct in_addr *)&myaddr.sin_addr.s_addr);
    myaddr.sin_port = htons(config.myport);

    if (bind(sock, (struct sockaddr *) &myaddr, sizeof(myaddr)) == -1) {
        perror("peer_run could not bind socket");
        exit(-1);
    }

    spiffy_init(config.identity, (struct sockaddr *)&myaddr, sizeof(myaddr));
    config.sock = sock;
    /* load my local chunk file list */
    init_hasChunk(config.has_chunk_file);

    while (1) {
        int nfds;
        FD_SET(STDIN_FILENO, &readfds);
        FD_SET(sock, &readfds);

        nfds = select(sock+1, &readfds, NULL, NULL, NULL);

        if (nfds > 0) {
            if (FD_ISSET(sock, &readfds)) {
                process_inbound_udp(sock);
            }

            if (FD_ISSET(STDIN_FILENO, &readfds)) {
                process_user_input(STDIN_FILENO, userbuf, handle_user_input,
                 "Currently unused");
            }
        }
    }
}

void init_hasChunk(char* has_chunk_file) {

    FILE* file = fopen(has_chunk_file,"r");
    char read_buffer[BUF_SIZE];
    char hash_buffer[SHA1_HASH_SIZE*2];
    
    hasChunk = queue_init();
    
    while (fgets(read_buffer,BUF_SIZE,file)) {
        chunk_t* chunk = malloc(sizeof(chunk_t));
        sscanf(read_buffer,"%d %s",&(chunk->id),hash_buffer);
        
        /* convert ascii to binary hash code */
        hex2binary(hash_buffer,SHA1_HASH_SIZE*2,chunk->hash);
        //fprintf(stderr, "ID: %d\n", chunk->id);
        //print_hash((uint8_t *)chunk->hash);
        enqueue(hasChunk, (void *)chunk);

        memset(read_buffer,0,BUF_SIZE);
        memset(hash_buffer,0,SHA1_HASH_SIZE*2);
    } 
    fclose(file);  

}