#include "p2pnet.h"

#include <sys/socket.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>

int p2pclient_init(p2pclient *client, p2pstate *state)
{
	client->state = state;

	return 0;
}


int p2pclient_send(p2pclient *c, p2pnode *node, char *msg, int lenmsg, char **response, int *lenresp)
{
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in addr;

	p2pheader hdr;

	p2phdr_create(&hdr, 0, 0, node, lenmsg);

	memset(&addr, 0, sizeof(struct sockaddr_in));
	addr.sin_port = htons(p2pstate_getapp(c->state)->port);


	addr.sin_family = AF_INET;
	addr.sin_addr = node->gateway;

	int r = connect(sock, &addr, sizeof(struct sockaddr_in));


	r = send(sock, &hdr, sizeof(p2pheader), NULL);

	/* Send main message */
	r = send(sock, msg, lenmsg, NULL);



	/* Wait for a response */
	if(response != NULL){
		//r = recv();
	}

	/* TODO: Eventually have persistent sockets to servers for frequently used connections */
	close(sock);

}

int p2pclient_clean(p2pclient *c)
{

}

