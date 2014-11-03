#################################################################################
# README                                                                       #
#                                                                              #
# Description: This file serves as a README and documentation for CP1 starter. #
#                                                                              #
# Authors: Yao Zhou <yaozhou@cs.cmu.edu>,                                      #
#          Ming Fang <mingf@cs.cmu.edu>                                        #
#                                                                              #
################################################################################




[TOC-1] Table of Contents
--------------------------------------------------------------------------------

        [TOC-1] Table of Contents
        [DES-2] Description of Files
        [DES-3] Description of Design for checkpoint1
        [DES-4] Description of Design for checkpoint2



[DES-2] Description of Files
--------------------------------------------------------------------------------

Here is a listing of all files associated with Recitation 1 and what their'
purpose is:

     .../readme.txt              - Current document 
     .../vulnerabilities.txt     - File contains the potential vulnerabilitis of this project
     .../test.txt                - File contains test case
     .../bt_parse.[c.h]          - Parse command line and config file
     .../chunk.[c.h]             - Process chunks
     .../client.c                - A simple client which is used to test UDP connection
     .../debug-text.h            - A file contains debug type 
     .../debug.[c.h]             - Helpful utilities for debugging output
     .../debugparse.pl           - Parse a debug.h file to create a debug-text.h file
     .../hupsim.pl               - This file emulates a network topology using topo.map
     .../input_buffer.[c|h]      - Handle user input
     .../job.[c|h]               - Process current job(downloading|uploading)
     .../timer.[c|h]             - Helper functions for timing
     .../conn.[c|h]              - Functions and structs that maintain connections
     .../make_chunks.c           - Source code for make-chunks executable fils
     .../nodes.map               - A map contains all nodes in the network
     .../peer.c                  - Process peers(main function)
     .../queue.[c|h]             - A generic queue  
     .../server.c                - A simple s   erver which is used to test UDP connection
     .../sha.c.[c|h]             - SHA-1 hash generator
     .../spiffy.[c|h]            - A simple network simulator
     .../test_input_buffer.c     - A simple test for user input
     .../topo.map                - A map file contains the network topology
     .../Makefile                - Contains rules for make




[DES-3] Description of Design for checkpoint1
--------------------------------------------------------------------------------
This project implements a peer to peer download applicaiton. By checkpoint 1, we have 
implemented the following functionalities:
1 Peer initialized with the knowledge of self information including identity hasChunks, 
  other reachable peers.
2 Peer is able to receive GET command typed on the comamand line.
3 Peer is able to parse the GET command and send WHOHAS packet to all reachable peers to
  discover available data
4 Peer is able to generate IHAVE response to received WHOHAS packet and send back. Peer
  does not send back IHAVE response if it does not have any requested data

[DES-4] Description of Design for checkpoint2
--------------------------------------------------------------------------------
1 When a peer receive IHAVE response, it is able to generate corresponding GET requests
  and send then one after one.
2 When a peer receive GET request, it is able to generate corresponding DATA packets and
  send them under the constraint of congestion control.
3 A sending peer is able to maintain a congestion window, which has an initial value of 8
  and a ssthresh of 64. The window size is changing at the time of receiving ACK
  packet and the changing value depends on the current state of sending peer, which is either
  slow start or congestion avoidance.
4 When a peer receive a DATA packet, it is able to, according to the seqence number of DATA
  packet, generate corresponding ACK packet and decide whether to store the data or drop it.
5 A peer is able to maintain a upload pool and a download pool with given number of connections
