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

	while(1){
		conn = accept(p->sock, &addr, &len);

		r = recv(conn, &hdr, sizeof(p2pheader), 0);
		if(r == -1){

		}

		if(r != sizeof(p2pheader)) {

		}

		/* Check the header to determing the size of the payload and how to store it */

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


/* TODO: DRY out the close()'s */
int p2pserv_start(p2pserver *serv)
{
	/* See if already started */
	if(serv->thread) {
		return 0;
	}

	/* Set up the socket */
	serv->sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(serv->sock == -1){
		return 1;
	}

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
	/* IF running */
		/* Close the socket and wait for the thread to terminate */
	/* clean up thread/running variables */
}

int p2pserv_clean(p2pserver *serv)
{

}
