
/* p2pbroadcaster - A UDP broadcasting based discovery server */

#include "p2pbroad.h"


#include <errno.h>


void p2pbroad_send(p2pbc *bc)
{
	int sockfd;
	struct sockaddr_in their_addr; // connector's address information
	struct hostent *he;
	int numbytes;
	int broadcast = 1;
	//char broadcast = '1'; // if that doesn't work, try this


	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
		perror("socket");
		exit(1);
	}

	// this call is what allows broadcast packets to be sent:
	if (setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof broadcast) == -1) {
			perror("setsockopt (SO_BROADCAST)");
			exit(1);
	}

	their_addr.sin_family = AF_INET;     // host byte order
	their_addr.sin_port = htons(ROOT_PORT); // short, network byte order
	//their_addr.sin_addr = *((struct in_addr *)he->h_addr);
	their_addr.sin_addr.s_addr = INADDR_BROADCAST;

	memset(their_addr.sin_zero, '\0', sizeof their_addr.sin_zero);


	p2pheader hdr;

	p2pnode node;

	p2phdr_create(&hdr, 0, P2P_MSG_WHO, &node, sizeof(p2pnode));


	/* Send header */
	if((numbytes=sendto(sockfd, &hdr, sizeof(p2pheader), 0, (struct sockaddr *)&their_addr, sizeof their_addr)) == -1) {
		perror("sendto1");
		exit(1);
	}

	if((numbytes=sendto(sockfd, &bc->state->self, sizeof(p2pnode), 0, (struct sockaddr *)&their_addr, sizeof their_addr)) == -1) {
		perror("sendto2");
		exit(1);
	}


	//printf("sent %d bytes to %s\n", numbytes,
	//inet_ntoa(their_addr.sin_addr));

	close(sockfd);
}


int p2pbroad_init(p2pbc *bc, p2pstate *s, P2PRECEIVER rcv)
{
	bc->state = s;
	return p2pserv_init(&bc->serv, s,  rcv);
}

int p2pbroad_start(p2pbc *bc)
{
	return p2pserv_start2(&bc->serv, P2P_UDP);
}
