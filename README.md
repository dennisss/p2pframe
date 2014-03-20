Peer-to-peer Framework
======================

Overview
--------
This is a framework for developing multi-computer terminal based applications. Some of the things that are taken care of by the framework are as follows:

* Creating/managing networks of computers
* Establishing/maintaining connections for one or more computers
* Common Application Layer protocol for sending network messages

Running/Building
----------------
* "make" will build shared code (p2pclient.c, p2pserver.c, p2pstate.c, p2pproto.c etc) to "bin/libp2pframe.a". It will also generate the main framework executable "bin/p2pframe" from its entry point in "p2pframe.c"

* "make bin/*" will build test code in "tests/%.c" and put it into "bin"

* "make bin/testapp" will build an included test that simply uses p2pserver/p2pclient to send data to itself in order to make sure that the code works

* "make bin/testchat" will build a test app that can be used with the framework to do simple chat between a client and server computer. Note: both ends of the program must be running on different computers with different IPs

To run execute "bin/p2pframe" on two computers on the same network. Type "list" on either computer to view a list of available other clients
Enter the name of the executable to be run (usually either bin/testapp or bin/testchat) and then enter the number of the node that should be connected to from "list". If the app should host a session instead of connect, specify a blank node value.

Technical Details
----------------------------

1. The framework starts as a standalone executable with entry point in p2pframe.c
   This binary will have:
    - a CLI - shows the user network stats, and allows them to actively make requests to other computers and start applications
	- a p2pstate - a structure allocated in shared memory that stores data like what computers are on the network etc. It will be shared with child applications
    - a p2pserver - this the root server. listens on a globally common port for requests from other computer's p2pclients, and then either replies to certain messages, or gives them to the commandline part for decisions (ie. receives a request to connect to a remote application: needs user approval)
	- a p2pclient - primarily works for the CLI by doing the work of sending messages.
	- a p2pbroadcaster - this will be a simple component unique to the root framework. it will run a udp server and client on the root port. it will broadcast messages to ALL LAN computers and see if their udp servers respond with the identify of the node (this will be the primarily way of discovering other users on the network and populating the p2pstate)

TODO: Change this one
2. When requested by either the CLI or the server, the framework will spawn a new process of an application binary
	- The application will first attach to the shared memory in the framework containing the p2pstate. This will allow it to query the network.
	- The application will wait for a connection, or if the state has a populated connection it will connect to that
	
3. The framework and application will continue to run more or less independently, and the framework will wait for step 1 to happen again
	
	
All network communications will run using the same data protocol (even the p2pbroadcaster). This will mean a great amount of flexibility, because any server or client can perform any supported request.
- The specific details of the data structures and format of messages in this protocol are outlined in p2pproto.h

Several components such as the p2pclient, p2pserver, protocol, and p2pstate code will be shared between applications and the framework. This is intended so that less code has to be written. I call the root framework binary the "root" one because it can be veiwed as an instance of an application itself, except with no parent.
