#ifndef P2PFRAME_H_
#define P2PFRAME_H_

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>

#include <pthread.h>

//TODO: This block can be removed
#ifndef bool
typedef int bool;
#define true 1
#define false 0
#endif

#define NAME_LENGTH 128

/* The port on which the main server will listen */
#define ROOT_PORT 1234

/* Defined in p2pproto.h */
//typedef struct p2pmsg;

/*
 * Used to uniquely identify a computer in the network
 *
 * On receiving a message, this uid will be compared to the one in state->self to see if
 * the message was meant for the computer that received it.
 *
 * A completely NULL uuid will indicate that the message is a broadcast,
 * and should be accepted by any computer that receives it.
 */
typedef struct {
	char vals[8]; /* 64 once randomly generated id. used to address the computer */
}uuid_t;

void generateuuid(uuid_t *buffer);

/* Kind of like strcmp, but for uid's */
int uuidcmp(const uuid_t *a, const uuid_t *b );


typedef struct {
	uuid_t uuid;
	struct in_addr gateway; /* The real IPV4 location of the node, or at least the location of a proxy to it */

	char name[NAME_LENGTH];

} p2pnode;

/* Stores data about a spawned instance of an app */
typedef struct {
	char name[NAME_LENGTH];

	/*
	 * Apps will be assigned local network ports. They will create servers that listen on this port
	 * Then the root server will communicate this port to other computers trying to connect to the app
	 *
	 * NOTE: For simplicity, all computers running the app will be listening to and talking to the same port.
	 * The port should probably be passed on a hash value of the app name in order to avoid collisions between apps
	 */
	/* TODO: Considering that both parties can calculate this port, should it be transmitted to the other computer?
	 * or is a ack needed at the least */
	int port;

	/*
	 * Process id
	 * This is used by the p2pserver/p2pclient components to internally determine which app is calling it and route networking appropriately
	 */
	pid_t pid;


	/* USed primarily by onconnection()/waitforconnection() to tell the application when a new connection has been established */
	//mqd_t msgqueue;

	/* The number of elements that are used in connections */
	int nconnections;

	/* Holds info about what nodes an app is connected to so that it can send/receive messages properly */
	/*struct {
		int node;
		int port;
	}*/ int connections[8]; /* Node indexes */

} p2papp;

/* TODO: Which header file should I put this in */
/* TODO: Add helper functions to modify the state */
/* The information that is needed by a p2pclient or p2pserver to work with the network */
/* NOTE: Pointers in the p2pstate and in member structures will actually be cursors relative to the start of the shared memory */
#define MAX_NODES 16
#define MAX_APPS 8
typedef struct {
	/* Information on the computer it is running on. */
	p2pnode self;

	/* Need to store data such as current connections, port assignments, etc. */
	int napps;
	p2papp apps[8];

	int nnodes;
	p2pnode nodes[16];


} p2pstate;


/* Generate the entry for a new app, and return the index. NOTE: The root framework will have index zero */
int p2pstate_newapp(p2pstate *s, char *name);



/* Should be called before execve() when trying to start a child application so that any connections are allocated  */
//void p2pstate_prepapp(p2pstate *s, p2pnode *connections /* TODO: Change this */);



/* TODO: Replace this with a macro called something like "APP" */
/* Returns the pointer to the calling app, or NULL on failure */
/* TODO: The framework may preload connections into app array, so it is important that only one one of these runs at a time */
/* TODO: Or the array entry can be preloaded after fork() but before execve() so that the data is alloted to the proper process id */
p2papp *p2pstate_getapp(p2pstate *s);
/* Returns the index of the current app or -1 on failure */
int p2pstate_getappid(p2pstate *s);

/* TODO: Should this be cached so that hostfullness is preserved or should every application loop through the connections to try to connect, only after that would it turn to host mode */
/* Checks to see if any connections have been registered to the app, if none, then the app has started in host mode, else it should contact that host to know whats up */
bool p2pstate_ishost();

/* Attempt to remove the app entry, this will only succeed if the associated pid is exited */
bool p2pstate_removeapp(p2pstate *s, int app);

void p2p_waitforconnection(p2pstate *s);
void p2p_onconnection(p2pstate* s, void (*onconnect)(int node));

/* Adds a connection to a given node on an app */
int p2pstate_addconnection(p2pstate *s, int app, int n);

/* Add and remove computers/nodes for the network */
int p2pstate_addnode(p2pstate *s, p2pnode *n);
void p2pstate_removenode(p2pstate *s, p2pnode *n);

/*
 * Prepares the p2pstate structure for use
 *
 * In the case of new being true, a new state will be initialized in shared memory. This case will occur only in the root server.
 * In the case of new being false, the state created by the root program will be attached.
 *
 * @param state a pointer to a pointer which will hold the final address the created state
 * @param new whether or not a new state is being created
 * @return zero on success
 */
int p2pinit(p2pstate **state, bool new);


#endif /* P2PFRAME_H_ */
