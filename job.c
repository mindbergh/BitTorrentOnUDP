#include "job.h"
#include "chunk.h"
#include "sha.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>


extern bt_config_t config;
extern job_t job;

int init_job(char* chunkFile) {
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
    
    job.num_chunk = line_number;
    job.chunks = malloc(sizeof(chunk_t) * job.num_chunk);
    
    /* set ptr to the beginning */
    fseek(file,0,SEEK_SET);
    
    while (fgets(read_buffer,BUF_SIZE,file)) {
        job.chunks[i] = malloc(sizeof(chunk_t));
        sscanf(buf,"%d %s",&(job.chunks[i].id),hash_buffer);
        
        /* convert ascii to binary hash code */
        hex2binary(hash_buffer,SHA1_HASH_SIZE*2,job.chunks[i].hash);
        
        memset(read_buffer,0,BUF_SIZE);
        memset(hash_buffer,0,SHA1_HASH_SIZE*2);
        i++;
    }    
    flose(file);
}

int if_Finished(job_t * job) {
    int i = 0;
    while(i < job.num_chunk) {
        if(job[i]->data == NULL)
            return -1;
        i++;
    }
    return 0;
}

int packet_parser(char* buf) {

    data_packet* data = (data_packet*) buf;
    /* check magic number */
    if( data->header->magicnum != 15651)
        return -1;
    if( data->header->version != 1)
        return -1;
    if( data->header->packet_type < 0 || data->header->packet_type > 5)
        return -1;
    
    return data->header->packet_type;

}

/** @brief Generate WhoHas package
 *  @return NULL if current job.num_chunk is 0
 *          a queue of pkt if job.num_chunk is not 0
 */
queue_t *WhoHas_maker() {
    // to do parse getchunkfile
    queue_t *q = queue_init();
    data_packet_t *pkt;
    char data[DATALEN];
    short pkt_len;
    int i, j, n, m;

    if (0 == job.num_chunk) {
        free_queue(q);
        return NULL;
    } else if (job.num_chunk > MAX_CHUNK) {
        n = job.num_chunk / MAX_CHUNK; /* multiple of 74 */
        for (i = 0; i < n; i++) {
            pkt_len = HEADERLEN + 4 + MAX_CHUNK * SHA1_HASH_SIZE;
            data = whohas_data_maker(MAX_CHUNK, job.chunks + i * MAX_CHUNK);
            pkt = packet_maker(PKT_WHOHAS, pkt_len, 0, 0, data);
            enqueue(q, (void *)pkt);
        }
        m = job.num_chunk - n * MAX_CHUNK; /* number of chunks can fit into one pkt */
        pkt_len = HEADERLEN + 4 + m * SHA1_HASH_SIZE;
        whohas_data_maker(m, job.chunks + n * MAX_CHUNK, data);
        pkt = packet_maker(PKT_WHOHAS, pkt_len, 0, 0, data);
        enqueue(q, (void *)pkt);
    } else {
        pkt_len = HEADERLEN + 4 + job.num_chunk * SHA1_HASH_SIZE;
        whohas_data_maker(job.num_chunk, job.chunks, data);
        pkt = packet_maker(PKT_WHOHAS, pkt_len, 0, 0, data);
        enqueue(q, (void *)pkt); 
    }
    return q;
}

/** @brief Generate WhoHas data part
 *  @param num_chunk the number of chunk requested
 *  @param chunks chunk_t that contian hash
 *  @param data an space to fillint data
 *  @return void
 */
void whohas_data_maker(int num_chunk, chunk_t *chunks, char* data) {
    char *data;
    char *ptr;
    short pkt_len;
    int i;
    
    /* Header:16 + num,padding:4 + hashs:20*num of chunk */
    pkt_len = HEADERLEN + 4 + num_chunk * SHA1_HASH_SIZE;
    data[0] = num_chunk; /* Number of chunks */
    memset(data[1], 0, 3); /* padding 3 bytes */
    ptr = data + 4;        /* shift 4 to fill in chunk hashs */
    for (i = 0; i < num_chunk; i++) {
        /* Chunk Hash 20 bytes */
        memcpy(ptr + i * SHA1_HASH_SIZE, chunks[i].hash, SHA1_HASH_SIZE);
    }
}

/** @brief Generate IHave data part
 *  @param whohas_pkt the recieved WhoHas pkt
 *  @return a pointer to data_packet_t generated
 */
data_packet_t *IHave_maker(data_packet_t *whohas_pkt) {
    // how many chunks are requested
    // how many of them i have
    // make IHave packet
    // return Ihave packet
    int req_num;
    int data_length = 4; // length of data including num and padding
    int have_num = 0;   // the num of chk I have 
    int i;
    char rawdata[PACKETLEN];
    char *hash_start;
    data_packet_t *pkt;

    assert(whohas_pkt->header.packet_type == PKT_WHOHAS);

    req_num = whohas_pkt->data[0];
    hash_start = &whohas_pkt->data[4];
    for (i = 0; i < req_num; i++) {
        if (IfIHave(hash_start)) {
            data_length += SHA1_HASH_SIZE;
            have_num++;
            memcpy(rawdata+n, hash_start, SHA1_HASH_SIZE);
        }
        hash_start += SHA1_HASH_SIZE;
    }
    if (0 == have_num)
        return NULL;  // I dont have any of you request chk
    
    memset(rawdata, 0, 4);
    rawdata[0] = have_num;
    pkt = packet_maker(PKT_IHAVE, HEADERLEN + data_length, 0, 0, rawdata);
    return pkt;
}


/** @brief IF I have the requested chunk
 *  @param hash_start requested hash that I need to cmp with what I have
 *  @return 1 if I have some of them
 *          0 if I dont have any one
 */
int IfIHave(uint8_t *hash_start) {
    int i;
    node_t *node;
    if (hasChunk.n == 0)
        return 0;
    node = hashChunk.head;
    for (i = 0; i < hasChunk.n; i++) {
        if (memcmp(hash_start, node->data, SHA1_HASH_SIZE) {
            node = node->next;
            continue;
        }                
        return 1
    }
    return 0;
}


/** @brief Generate GET pkt
 *  @param pkt incoming Ihave pkt
 *  @param provider the incoming peer who send the IHave
 *  @return NULL I dont need to send GET this provider.
 *          a queue of GET pkt
 */
queue_t* GET_maker(data_packet_t *pkt, bt_peer_t* provider) {
    assert(whohas_pkt->header.packet_type == PKT_IHAVE);
    int num = pkt->data[0]; // num of chunk that peer has
    int num_match = 0;
    int i;
    int match_id;
    chunk_t* chk = job.chunks;  // the needed chunk here
    queue_t *q;      // the queue of GET request
    data_packet_t* pkt; // GET packet
    uint8_t *hash;   // the incoming hash waiting to match my needs
    if (0 == num)
        return NULL;

    q = queue_init();
    hash = pkt->data[4]; // the start of hash
    for (i = 0; i < num; i++) {
        match_id = match_need(hash);
        if (-1 != match_id) {
            chk[i].pvd = provider;
            chk[i].num_p = 1;
            pkt = packet_maker(PKT_GET, HEADERLEN + SHA1_HASH_SIZE, 0, 0, hash);
            enqueue(q, (void *)pkt);
        }
        hash += SHA1_HASH_SIZE;
    }
    return q;
}


/** @brief try to match the incoming IHave to what I want
 *  @param hash the incoming chunk hash to be matched
 *  @return -1 if I dont need this
 *          the int id of needed chunk that matches this hash
 */
int match_need(uint8_t *hash) {
    int i;
    chunk_t* chk = job.chunks;
    if (0 == job.num_chunk)
        return NULL;
    for (i = 0; i < job.num_chunk; i++) {
        if (NULL != chk[i].pvd)
            continue;
        if (memcmp(hash, chk[i].hash, SHA1_HASH_SIZE) {
            continue;
        } else return i;        
    }
    return -1;
}


/** @brief Generate package
 *  @param type the type of pkt WHOHAS, IHAVE, GET, DATA, ACK, DENIED
 *  @param pkt_len the length of pkt including header
 *  @param seq sequence num
 *  @param ack ack num
 *  @param data the pointer to data (memcpy needed)
 *  @return pointer to the pkt genereted
 */
data_packet_t *packet_maker(int type, short pkt_len, u_int seq, u_int ack, char *data) {
    data_packet_t *pkt = (data_packet_t *)malloc(sizeof(data_packet_t));
    pkt->header.magicnum = 15441; /* Magic number */
    pkt->header.version = 1;      /* Version number */
    pkt->header.packet_type = type; /* Packet Type */
    pkt->header.header_len = HEADERLEN;    /* Header length is always 16 */
    pkt->header.packet_len = pkt_len;
    pkt->header.seq_num = seq;
    pkt->header.ack_num = ack;
    memcpy(pkt->data, data, pkt_len - HEADERLEN);
    return pkt;
}

/** @brief Send whohas pkt to all peers in the network
 *  @param pkt pkt to be send
 *  @return void
 */
void Send_WhoHas(data_packet_t* pkt) {
    
    bt_peer_s* peer = config.peers;
    while(peer != NULL) {
        packet_sender(pkt,(struct sockaddr *)peer->addr);
        peer = peer.next;
    }
}



void packet_sender(data_packet_t* pkt, struct sockaddr* to) {
    spiffy_sendto(config->myport, pkt, pkt.header.packet_len, 0, to, sizeof(*to));
}

/** @brief free pkt
 *  @param pkt the pkt to be freed
 *  @return void
 */
void packet_free(data_packet_t *pkt) {
    //free(pkt->data);
    free(pkt);
}

/** @brief Print out the given pkt
 *  @param pkt the pkt to to printed out
 *  @return void
 */
void print_pkt(data_packet_t* pkt) {
    header_t* hdr = pkt.header;
    uint8_t* hash;
    int num;
    int i;
    fprintf(stderr, "magicnum:\t\t%d\n", hdr->magicnum);
    fprintf(stderr, "version:\t\t%d\n", hdr->version);
    fprintf(stderr, "packet_type:\t\t%d\n", hdr->packet_type);
    fprintf(stderr, "header_len:\t\t%d\n", hdr->header_len);
    fprintf(stderr, "packet_len:\t\t%d\n", hdr->packet_len);
    fprintf(stderr, "seq_num:\t\t%d\n", hdr->seq_num);
    fprintf(stderr, "ack_num:\t\t%d\n", hdr->ack_num);
    if (PKT_WHOHAS = hdr->packet_type || PKT_IHAVE == hdr->packet_type) {
        num = pkt->data[0];
        fprintf(stderr, "1st bytes data:\t\t%x\n", pkt->data[0]);
        hash = (uint8_t *)pkt->data[4];
        for (i = 0; i < num; i++) {
            print_hash(hash);
            hash += SHA1_HASH_SIZE;
        }
    }
}

/** @brief Print out hash
 *  @param hash the pointer to the hash to be printed out
 *  @return void
 */
void print_hash(uint8_t *hash) {
    for (i = 0; i < SHA1_HASH_SIZE;) {
        fprintf(stderr, "%02x", hash[i++]);
        if (!(i % 4))
            fprintf(stderr, " ");
    }
    fprintf(stderr, "\n");
}

void freeJob(job_t* job) {

    int i = 0;
    /* free each chunks */
    while (i < job.num_chunk) {
        free((job.chunks[i])->data);
        free((job.chunks[i])->p);
        free(job.chunks[i]);
        i++;
    }
    job.chunks = NULL;
    job.num_chunk = 0;
}

