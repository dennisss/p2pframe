#ifndef P2PFRAME_H_
#define P2PFRAME_H_

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>


/* The port on which the main server will listen */
#define ROOT_PORT 1234


/*
 * Used to uniquely identify a computer in the network
 *
 * On receiving a message, this uid will be compared to the one in state->self to see if
 * the message was meant for the computer that received it.
 *
 * A completely NULL uid will indicate that the message is a broadcast,
 * and should be accepted by any computer that receives it.
 */
typedef struct {
	char vals[8]; /* 64 once randomly generated id. used to address the computer */
}uid_t;

void generateuid(uid_t *buffer);

/* Kind of like strcmp, but for uid's */
int uidcmp(const uid_t *a, const uid_t *b );


typedef struct {
	uid_t uid;
	in_addr gateway; /* The real IPV4 location of the node, or at least the location of a proxy to it */

	char *name;

} p2pnode;

/* Stores data about a spawned instance of an app */
typedef struct {
	char *name;

	/*
	 * Apps will be assigned local network ports. They will create servers that listen on this port
	 * Then the root server will communicate this port to other computers trying to connect to the app
	 */
	int port;

	/* Process id */
	pid_t pid;

} p2papp;

/* TODO: Which header file should I put this in */
/* TODO: Add helper functions to modify the state */
/* The information that is needed by a p2pclient or p2pserver to work with the network */
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


void p2pstate_addnode(p2pstate *s, p2pnode *n);
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
