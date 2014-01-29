/* This file is the entry point for the root server. It will keep track of the network, manage/sync applications, and spawn them when needed. */

#include "p2pframe.h"
#include <ncurses.h>

/* This will be a local network UDP server that will attempt to discover other servers on the LAN and also respond to any of their requests */
typedef struct {


} p2pbroadcaster;



p2pstate *state;
p2pserver rootserver;
p2pclient rootclient;

void

int main(int argc, char argv[])
{
	/* TODO: This should eventually by allocated in separate function and in a shared memory region */
	state = (p2pstate *)malloc(sizeof(p2pstate));
	state->napps = 0;
	state->nnodes = 0;


	/* Terminal/ncurses stuff here */


	return 0;

}
