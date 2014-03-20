/* This file is the entry point for the root server. It will keep track of the network, manage/sync applications, and spawn them when needed. */

#include "p2pnet.h"
#include "p2pframe.h"
//#include <ncurses.h>
#include "p2pbroad.h"

#include <unistd.h>

/* For adding connection */
#include <netinet/in.h>

p2pstate *state;
p2pserver rootserver;
p2pclient rootclient;

p2pbc broadcaster;

void startapp(char *path, bool host, int connection)
{
	char appPath[512];


	pid_t pid;

	if((pid = fork()) == NULL){ /* Child */

		/* Create the app and populate it with connections if needed */
		int app = p2pstate_newapp(state, "Unnamed");


		if(host){

		}
		else{
			p2pstate_addconnection(state, app, connection);
		}


		getcwd(appPath, sizeof(appPath));

		strcat(appPath, "/");
		strcat(appPath, path);

		printf("Starting %s\n", appPath);

		//execve(path, path /* TODO: Change this */);
		execl(appPath, path, 0);


		printf("Failed to load application\n");

		exit(1);

	}
	else{
		/* Parent */
		int status;
		waitpid(pid, &status, WUNTRACED);


		/* Clean up the app entry here */
	}


}

/* This can work as the reeiver for both the UDP and TCP servers */
/* Also for the broadcaster: Listens for friendship requests/pings, stores the identity of the requesting computer as a node and send back the local computer's identity */
/* TODO: Make sure that all the arguments passed to the receiver are implemented */
void root_receiver(char *msg, int lenmsg, int type, int sender)
{

	/*
	P2P_MSG_HEARTBEAT 1
	P2P_MSG_FTP
	P2P_MSG_WHO
	P2P_MSG_ID
	P2P_MSG_ACK
	P2P_MSG_DATA
	 */

	if(type == P2P_MSG_WHO){
		/* Send back this computer's id */

		if(lenmsg != sizeof(p2pnode)){
			printf("bad node received\n");
		}

		p2pnode *n = (p2pnode *)msg;

		strcpy(&state->nodes[sender].name, &n->name);


		char *m = "hello";
		/* For now this will just send a blank message back to the other client so that it knows that we exist */
		p2pclient_send(&rootclient, &state->nodes[sender], m,strlen(m),0,0);



		/* Ignore messages sent by itself */
		/*if(uuidcmp(&n->uuid, &state->self.uuid) == 0){


		}*/

		///printf("got a who is\n");

	}

	if(type == P2P_MSG_ID){

		/* Store the ID of another server */

	}

	if(type == P2P_MSG_CONNECT){
		/* Do an acknowledgment on the external client's request to connect */
			/* Fill the connections array appropriately */
			/* Send over any needed files */
	}

	if(type == P2P_MSG_QUERY){
		/* Return what apps the server is running and other neat states maybe... */

	}

	/* If a connect request message: */
		/* If the app is NOT started */
			/* Create new app and starts it  */

		/* App waits for connection request from the remote app */
		/* Apps keep going back and forth depending on what they are doing */

}

int main(int argc, char argv[])
{

	if(p2pinit(&state, true) != 0){
		printf("Failed to initialize p2p state\n");
		exit(1);
	}

	int app = p2pstate_newapp(state, "p2pframework");


	if(p2pserv_init(&rootserver, state, root_receiver) != 0){
		printf("Failed to initialize p2p server\n");
		exit(1);
	}

	if(p2pserv_start(&rootserver) != 0){
		printf("Failed to start p2p server\n");
		exit(1);

	}

	printf("Name: ");
	fflush(stdin);
	fgets(state->self.name, 128, stdin);

	if(state->self.name[strlen(state->self.name) - 1] == '\n')
		state->self.name[strlen(state->self.name) - 1] = 0;


	if(p2pclient_init(&rootclient, state) != 0){
		printf("Failed to initialize p2p client\n");
		exit(1);
	}

	p2pbroad_init(&broadcaster, state, root_receiver);
	p2pbroad_start(&broadcaster);
	//while(1){
		p2pbroad_send(&broadcaster);
	//	sleep(1);
	//}
	//return 0;


	printf("Basic framework shell for starting programs\nEnter the name of the executable such as 'bin/testchat'");
	printf("Then enter an ip to connect to (leave it blank if it should run as a host/server)\n");
	printf("( or instead of the executable name, type 'quit' to end the program or 'list' to show computers on the net )\n");
	printf("----------------------------------------------------------\n");

	char buffer[256]; /* Store executable name */
	char buffer2[256];  /* Store ip or blank(newline) */

	while(1){
		printf("> ");
		fflush(stdout);

		fgets(buffer, 256, stdin);

		if(buffer[strlen(buffer) - 1] == '\n')
			buffer[strlen(buffer) - 1] = 0;

		if(strlen(buffer) == 0)
			continue;

		if(strcmp(buffer, "quit") == 0){
			break;
		}

		if(strcmp(buffer, "list") == 0){
			int i;
			for(i = 0; i < state->nnodes; i++){
				printf("%d:   %s %s\n", i, state->nodes[i].name, inet_ntoa(state->nodes[i].gateway.s_addr));
			}

			continue;
			/* TODO: Print out entries from state->nodes of length state->nnodes */
			/* Eventually that array will store a list of computers found on the network */
		}


		printf("node? ");
		fflush(stdout);

		fgets(buffer2, 256, stdin);

		if(buffer2[strlen(buffer2) - 1] == '\n')
			buffer2[strlen(buffer2) - 1] = 0;

		int nid = NULL;
		bool host = true;

		/* Give the client a preset server connection */
		if(strlen(buffer2) > 0){

			p2pnode nc;

			nid = atoi(buffer2);
			if(nid < 0 | nid >= state->nnodes){
				printf("Invalid node number, type 'list' to see all available ones\n");
				continue;
			}

			host = false;
		}


		startapp(buffer, host, nid);

	}



	p2pclient_clean(&rootclient);
	p2pserv_stop(&rootserver);
	p2pserv_clean(&rootserver);

	return 0;

}
