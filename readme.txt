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
<<<<<<< HEAD

=======
        [DES-4] Description of Design for checkpoint2
>>>>>>> upstream/master



[DES-2] Description of Files
--------------------------------------------------------------------------------

Here is a listing of all files associated with Recitation 1 and what their'
purpose is:

<<<<<<< HEAD
     .../readme.txt                  - Current document 
     .../vulnerabilities.txt         - File contains the potential vulnerabilitis of this project
     .../test.txt                    - File contains test case
     .../src/bt_parse.[c.h]          - Parse command line and config file
     .../src/chunk.[c.h]             - Process chunks
     .../src/client.c                - A simple client which is used to test UDP connection
     .../src/debug-text.h            - A file contains debug type 
     .../src/debug.[c.h]             - Helpful utilities for debugging output
     .../src/debugparse.pl           - Parse a debug.h file to create a debug-text.h file
     .../src/hupsim.pl               - This file emulates a network topology using topo.map
     .../src/input_buffer.[c|h]      - Handle user input
     .../src/job.[c|h]               - Process current job(downloading|uploading)
     .../src/make_chunks.c           - Source code for make-chunks executable fils
     .../src/nodes.map               - A map contains all nodes in the network
     .../src/peer.c                  - Process peers(main function)
     .../src/queue.[c|h]             - A generic queue  
     .../src/server.c                - A simple s   erver which is used to test UDP connection
     .../src/sha.c.[c|h]             - SHA-1 hash generator
     .../src/spiffy.[c|h]            - A simple network simulator
     .../src/test_input_buffer.c     - A simple test for user input
     .../src/topo.map                - A map file contains the network topology
     .../src/Makefile                - Contains rules for make
=======
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
>>>>>>> upstream/master




[DES-3] Description of Design for checkpoint1
--------------------------------------------------------------------------------
<<<<<<< HEAD
This project implements a peer to peer download applicaiton. By checkpoint 1, we have implemented
the following functionalities:
1 Peer initialized with the knowledge of self information including identity hasChunks, other reachable
  peers.
2 Peer is able to receive GET command typed on the comamand line.
3 Peer is able to parse the GET command and send WHOHAS packet to all reachable peers to discover available
  data
4 Peer is able to generate IHAVE response to received WHOHAS packet and send back. Peer does not send back
  IHAVE response if it does not have any requested data
=======
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
>>>>>>> upstream/master
