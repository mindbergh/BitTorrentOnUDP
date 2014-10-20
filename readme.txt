。################################################################################
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




[DES-2] Description of Files
--------------------------------------------------------------------------------

Here is a listing of all files associated with Recitation 1 and what their'
purpose is:

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




[DES-3] Description of Design for checkpoint1
--------------------------------------------------------------------------------
This project implements a peer to peer download applicaiton. By checkpoint 1, we have implemented
the following functionalities:
1 Peer initialized with the knowledge of self information including identity hasChunks, other reachable
  peers.
2 Peer is able to receive GET command typed on the comamand line.
3 Peer is able to parse the GET command and send WHOHAS packet to all reachable peers to discover available
  data
4 Peer is able to generate IHAVE response to received WHOHAS packet and send back. Peer does not send back
  IHAVE response if it does not have any requested data
