#include "../src/p2pnet.h"
#include "../src/p2pframe.h"

/* Testing a server-client relationship between apps through a simple oneway chat */


/* TODO: Move this include */
#include <arpa/inet.h>

p2pserver serv; /* Will wait for new information */
p2pclient client; /* Will send data to the p2pserver */
p2pstate *state; /* Required for all applications */

bool connected = false;

void receivemsg(char *msg, int lenmsg, int node)
{
	printf("Recv: ");

	connected = true;

	if(strcmp(msg, "Quit") == 0){
		printf("Exit command...\n");
		exit(0);
	}

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


	/* If there a connection in the app struct */
		/* Act in client mode and start talking to the server */

	if(state->apps[app].nconnections > 0 /* The connections array will contain any computers that the user requested that the application should connect to. In the case of hosting mode, the array is empty */){
		char *msg = "First Message";
		printf("Send: %s\n", msg);
		p2pclient_send(&client, & state->nodes[ state->apps[app].connections[0] ], msg, strlen(msg), NULL, NULL);
	}
	else{

		printf("Waiting for a connection...\n");

		while(!connected){
			sleep(1);
		}
		printf("Got a connection.\n");

		while(1){ /* Don't stop the program, just keep receiving stuff  */
			sleep(1);
		}
	}


	char buffer[256];

	while(true){
		scanf("%s", buffer);

		p2pclient_send(&client, & state->nodes[ state->apps[app].connections[0] ], buffer, strlen(buffer) + 1, NULL, NULL);

		if(strcmp(buffer, "Quit") == 0){
			break;
		}

	}



	p2pclient_clean(&client);
	p2pserv_stop(&serv);
	p2pserv_clean(&serv);
}
