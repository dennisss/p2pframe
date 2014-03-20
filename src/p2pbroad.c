
/* p2pbroadcaster - A UDP broadcasting based discovery server */

#include "p2pbroad.h"


#include <errno.h>

void broadcast_receiver(char *msg, int lenmsg, int type, int sender)
{
	printf("Broadcast received!!\n");
}

/* Listens for friendship requests/pings, stores the identity of the requesting computer as a node and send back the local computer's identity */
void *p2pbroad_listen(void *arg)
{

/*
    int sockfd;
    struct addrinfo hints, *servinfo, *p;
    int rv;
    int numbytes;
    struct sockaddr_storage their_addr;
    char buf[MAXBUFLEN];
    socklen_t addr_len;
    char s[INET6_ADDRSTRLEN];

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; // set to AF_INET to force IPv4
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    if ((rv = getaddrinfo(NULL, MYPORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("listener: socket");
            continue;
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("listener: bind");
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "listener: failed to bind socket\n");
        return 2;
    }

    freeaddrinfo(servinfo);

    printf("listener: waiting to recvfrom...\n");

    addr_len = sizeof their_addr;
    if ((numbytes = recvfrom(sockfd, buf, MAXBUFLEN-1 , 0,
        (struct sockaddr *)&their_addr, &addr_len)) == -1) {
        perror("recvfrom");
        exit(1);
    }

    printf("listener: got packet from %s\n",
        inet_ntop(their_addr.ss_family,
            get_in_addr((struct sockaddr *)&their_addr),
            s, sizeof s));
    printf("listener: packet is %d bytes long\n", numbytes);
    buf[numbytes] = '\0';
    printf("listener: packet contains \"%s\"\n", buf);

    close(sockfd);

    return 0;


 */

}

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
	//inet_aton("127.0.0.1", &their_addr.sin_addr.s_addr);
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

/*

	r = sendto(sock, &hdr, sizeof(p2pheader), NULL, &addr, sizeof(addr));
	printf("r: %d\n", errno);

	r = sendto(sock, &bc->state->self, sizeof(p2pnode), NULL, &addr, sizeof(addr));
	printf("r: %d\n", r);


 */


	printf("sent %d bytes to %s\n", numbytes,
	inet_ntoa(their_addr.sin_addr));

	close(sockfd);









	return;
/*

	int r;

	int sock = socket(AF_INET, SOCK_DGRAM, 0);
	if(sock == -1){
		return;
	}

	printf("a1\n");

	int broadcast = 0;

	if(setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast)) == -1){
		return;
	}

	printf("a2\n");

	p2pheader hdr;

	p2pnode node;

	p2phdr_create(&hdr, 0, P2P_MSG_WHO, &node, sizeof(p2pnode));


	struct sockaddr_in addr;

	addr.sin_addr.s_addr = INADDR_BROADCAST;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(ROOT_PORT);
	memset(addr.sin_zero, '\0', sizeof(addr.sin_zero));

	r = sendto(sock, &hdr, sizeof(p2pheader), NULL, &addr, sizeof(addr));
	printf("r: %d\n", errno);

	r = sendto(sock, &bc->state->self, sizeof(p2pnode), NULL, &addr, sizeof(addr));
	printf("r: %d\n", r);

	printf("yah!\n");

	close(sock);
*/

/*
	int sockfd;
   	struct sockaddr_in their_addr; // connector's address information
    struct hostent *he;
    int numbytes;
    int broadcast = 1;
    //char broadcast = '1'; // if that doesn't work, try this

    if (argc != 3) {
        fprintf(stderr,"usage: broadcaster hostname message\n");
        exit(1);
    }

    if ((he=gethostbyname(argv[1])) == NULL) {  // get the host info
        perror("gethostbyname");
        exit(1);
    }

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }

    // this call is what allows broadcast packets to be sent:
    if (setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &broadcast,
        sizeof broadcast) == -1) {
        perror("setsockopt (SO_BROADCAST)");
        exit(1);
    }

    their_addr.sin_family = AF_INET;     // host byte order
    their_addr.sin_port = htons(SERVERPORT); // short, network byte order
    their_addr.sin_addr = *((struct in_addr *)he->h_addr);
    memset(their_addr.sin_zero, '\0', sizeof their_addr.sin_zero);

    if ((numbytes=sendto(sockfd, argv[2], strlen(argv[2]), 0,
             (struct sockaddr *)&their_addr, sizeof their_addr)) == -1) {
        perror("sendto");
        exit(1);
    }

    printf("sent %d bytes to %s\n", numbytes,
        inet_ntoa(their_addr.sin_addr));

    close(sockfd);

    return 0;



 */



}


int p2pbroad_init(p2pbc *bc, p2pstate *s, P2PRECEIVER rcv)
{
	bc->state = s;
	return p2pserv_init(&bc->serv, s,  rcv);


	/*

	bc->sock = socket(AF_INET, SOCK_DGRAM, 0);
	if(bc->sock == -1){
		return 1;
	}


	struct sockaddr_in addr_in;
	addr_in.sin_family = AF_INET;
	addr_in.sin_addr.s_addr = INADDR_ANY;
	addr_in.sin_port = htons(serv->port);

	if(bind(serv->sock, &addr_in, sizeof(addr_in)) < 0){
		close(bc->sock);
		return 1;
	}



	serv->running = true;
	if(pthread_create(&serv->thread, NULL, p2pbroad_listen, bc)) {
		serv->thread = NULL;
		serv->running = false;
		close(serv->sock);
		return 1;
	}

	*/

}

int p2pbroad_start(p2pbc *bc)
{
	return p2pserv_start2(&bc->serv, P2P_UDP);


}
