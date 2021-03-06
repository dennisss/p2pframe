#include "p2pnet.h"

#include <sys/socket.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <netinet/in.h>


int p2pserv_init(p2pserver *serv, p2pstate *state, P2PRECEIVER rcv)
{
	serv->state = state;

	/* Port is based on which app is associated with the calling process */
	serv->port = p2pstate_getapp(serv->state)->port;

	serv->code = rcv;
	serv->thread = NULL;
	serv->running = false;

	return 0;
}


void *p2pserver_run(void *arg)
{
	p2pserver *p = (p2pserver *)arg;

	int conn, r, sender, i;
	struct sockaddr_in addr;
	socklen_t len = sizeof(struct sockaddr_in);

	p2pnode n;

	p2pheader hdr;
	char buffer[4096];

	while(p->running){ /* TODO: Make sure this stops properly  */

		sender = -1;

		if(p->proto == P2P_TCP){

			conn = accept(p->sock, &addr, &len);

			if(conn == -1){
				printf("problem!!!\n");
				/* Failed to connect, the socket was probably closed */
				break;
			}
		}
		else{
			/* Because UDP is connection less */
			conn = p->sock;
		}




		if(p->proto == P2P_TCP){
			r = recv(conn, &hdr, sizeof(p2pheader), 0);
		}
		else{
			r = recvfrom(conn, &hdr, sizeof(p2pheader), 0, &addr, &len);
		}

		if(r == -1){
			printf("failed!\n");
		}

		if(r != sizeof(p2pheader)) {
			printf("bad count while receiving\n");
		}

		/* TODO: Check the header to determining the size of the payload and how to store it */

		/* Read from the socket */
		/* TODO: For UDP make sure that the same computer sent the data */
		r = recv(conn, buffer, hdr.length /* TODO: Make sure that length < sizeof(buffer) */, 0);
		if(r == -1){
			/* Close connection, failure */
			printf("recv failure\n");
			close(conn);
			break;
		}

		if(r == 0){
			/* Nothing was read */
			close(conn);
			continue;
		}

		if(r >= sizeof(buffer)){
			/* Request is really really big, we will ignore it for now */
			close(conn);
			continue;

		}


		/* Setup the sender based on the address that sent the data */
		/* TODO: Eventually this process should be restricted to only run on the root framework so that clients can be denied if they aren't first properly registered */

		bool exists = false;
		for(i = 0; i < p->state->nnodes; i++){
			if(p->state->nodes[i].gateway.s_addr == addr.sin_addr.s_addr){
				exists = true;
				sender = i;
				break;
			}
		}

		int appid = p2pstate_getappid(p->state);

		/* Add the node to the network list if isn't already on it */
		if(!exists){
			n.gateway.s_addr = addr.sin_addr.s_addr;
			strcpy(n.name, "Anonymous Computer");
			sender = p2pstate_addnode(p->state, &n);



			//printf("added computer!\n");
		}

		if(appid != 0){
				p2pstate_addconnection(p->state, p2pstate_getappid(p->state), sender);
		}



		/* Send back the information to the receiver code */
		p->code(buffer, r, hdr.type, sender);


		/* TODO: If there is a response, send it back here */

		/* TODO: Put this check on all the close()'s in this function */
		if(p->proto == P2P_TCP){
			close(conn);
		}
	}


}

int p2pserv_start(p2pserver *serv){
	return p2pserv_start2(serv, P2P_TCP);
}

/* TODO: DRY out the close()'s */
/* Starts a server under either the protocol P2P_TCP or P2P_UDP */
int p2pserv_start2(p2pserver *serv, int proto)
{
	serv->proto = proto;

	/* See if already started */
	if(serv->running) {
		return 0;
	}

	/* Set up the socket */
	if(proto == P2P_TCP)
		serv->sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	else if(proto == P2P_UDP)
		serv->sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	else{
		printf("Invalid protocol\n");
		return 1;
	}

	if(serv->sock == -1){
		printf("Could not create socket\n");
		return 1;
	}

#ifdef SO_REUSEPORT
	int reuse = 1;
	if(setsockopt(serv->sock, SOL_SOCKET, SO_REUSEPORT, &reuse, sizeof(int)) == -1){
		close(serv->sock);
		printf("Failed to make socket reusable\n");
		return 1;
	}
#endif

	struct sockaddr_in addr_in;
	addr_in.sin_family = AF_INET;
	addr_in.sin_addr.s_addr = INADDR_ANY;
	addr_in.sin_port = htons(serv->port);


	if(bind(serv->sock, &addr_in, sizeof(addr_in)) < 0){
		close(serv->sock);
		printf("Failed to bind\n");
		return 1;
	}

	if(proto == P2P_TCP){
		if(listen(serv->sock, SOMAXCONN) < 0) {
			close(serv->sock);
			printf("Failed to listen\n");
			return 1;
		}
	}

	/* Start a new thread to handle incoming requests */
	serv->running = true;
	if(pthread_create(&serv->thread, NULL, p2pserver_run, serv)) {
		serv->thread = NULL;
		serv->running = false;
		close(serv->sock);
		return 1;
	}

	return 0;
}

int p2pserv_stop(p2pserver *serv)
{
	if(serv->running){
		/* This should force it to stop blocking on accept() */
		serv->running = false;
		close(serv->sock);

		pthread_kill(serv->thread, 0); /* TODO: Make sure that I do not need to null the serv->thread value */
		serv->thread = NULL;
	}
}

int p2pserv_clean(p2pserver *serv)
{

}
