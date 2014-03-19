#include "../src/p2pnet.h"
#include "../src/p2pframe.h"

/* Basic bare minimum test of p2pserver and p2pclient components*/


/* TODO: Move this include */
#include <arpa/inet.h>

p2pserver serv;
p2pclient client; /* Will send data to the p2pserver */
p2pstate *state; /* Required for all applications */

void receivemsg(char *msg, int lenmsg)
{
	printf("Received: ");

	printf("%s\n", msg);

}


int main(int argc, char *argv[])
{
	if(p2pinit(&state, false /* For actual sub-applications this will be false, but for testing it as a standalone, it should be true */) != 0){
		printf("Failed to initialize p2p state\n");
		exit(1);
	}


	int app = p2pstate_newapp(state, "Test App");


	if(p2pserv_init(&serv, state, receivemsg) != 0){
		printf("Failed to initialize p2p server\n");
		exit(1);
	}

	if(p2pserv_start(&serv) != 0){
		printf("Failed to start p2p server\n");
		exit(1);

	}


	if(p2pclient_init(&client, state) != 0){
		printf("Failed to initialize p2p client\n");
		exit(1);
	}


	sleep(1);


	printf("Sending: Test\n");
	p2pclient_send(&client, &state->self /* Send it to itself */, "Test", 5, NULL, NULL);

	sleep(2);


	p2pclient_clean(&client);
	p2pserv_stop(&serv);
	p2pserv_clean(&serv);
}
