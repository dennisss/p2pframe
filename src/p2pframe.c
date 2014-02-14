/* This file is the entry point for the root server. It will keep track of the network, manage/sync applications, and spawn them when needed. */

#include "p2pnet.h"
#include "p2pframe.h"
//#include <ncurses.h>

/* This will be a local network UDP server that will attempt to discover other servers on the LAN and also respond to any of their requests */
typedef struct {


} p2pbroadcaster;



p2pstate *state;
p2pserver rootserver;
p2pclient rootclient;

void root_receiver(char *msg, int lenmsg)
{
	/* If a connect request message: */
		/* If the app is NOT started */
			/* Create new app and starts it  */

		/* App waits for connection request from the remote app */
		/* Apps keep going back and forth depending on what they are doing */

}

int main(int argc, char argv[])
{
	if(p2pinit(&state, true /* For actual sub-applications this will be false */) != 0){
		printf("Failed to initialize p2p state\n");
		exit(1);
	}


	int app = p2pstate_newapp(state, "Test App");

	if(p2pserv_init(&rootserver, state, root_receiver) != 0){
		printf("Failed to initialize p2p server\n");
		exit(1);
	}

	if(p2pserv_start(&rootserver) != 0){
		printf("Failed to start p2p server\n");
		exit(1);

	}


	if(p2pclient_init(&rootclient, state) != 0){
		printf("Failed to initialize p2p client\n");
		exit(1);
	}



	/* Terminal/ncurses stuff here */
	printf("Hello World!");



	p2pclient_clean(&rootclient);
	p2pserv_stop(&rootserver);
	p2pserv_clean(&rootserver);

	return 0;

}
