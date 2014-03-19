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

	int conn, r;
	struct sockaddr_in addr;
	socklen_t len = sizeof(struct sockaddr_in);

	p2pheader hdr;
	char buffer[4096];

	while(p->running){ /* TODO: Make sure this stops properly  */

		conn = accept(p->sock, &addr, &len);

		if(conn == -1){
			printf("problem!!!\n");
			/* Failed to connect, the socket was probably closed */
			break;
		}

		r = recv(conn, &hdr, sizeof(p2pheader), 0);
		if(r == -1){
			printf("failed!\n");
		}

		if(r != sizeof(p2pheader)) {

		}

		/* Check the header to determining the size of the payload and how to store it */

		/* Read from the socket */
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

		p->code(buffer, r, hdr.type);


		/* TODO: If there is a response, send it back here */

		close(conn);
	}


}

int p2pserv_start(p2pserver *serv){
	return p2pserv_start2(serv, P2P_TCP);
}

/* TODO: DRY out the close()'s */
/* Starts a server under either the protocol P2P_TCP or P2P_UDP */
int p2pserv_start2(p2pserver *serv, int proto)
{
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
		return 1;
	}

#ifdef SO_REUSEPORT
	int reuse = 1;
	if(setsockopt(serv->sock, SOL_SOCKET, SO_REUSEPORT, &reuse, sizeof(int)) == -1){
		return 1;
	}
#endif

	struct sockaddr_in addr_in;
	addr_in.sin_family = AF_INET;
	addr_in.sin_addr.s_addr = INADDR_ANY;
	addr_in.sin_port = htons(serv->port);


	if(bind(serv->sock, &addr_in, sizeof(addr_in)) < 0){
		close(serv->sock);
		return 1;
	}

	if(listen(serv->sock, SOMAXCONN) < 0) {
		close(serv->sock);
		return 1;
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
