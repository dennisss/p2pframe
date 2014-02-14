#ifndef P2PNET_H_
#define P2PNET_H_

#include "p2pframe.h"
#include "p2pproto.h"

#include <pthread.h>

typedef void (*P2PRECEIVER) (char *msg, int len, int type);

/**
 * A p2pserver binds to a local port and waits for requests.
 *
 * If the server is the root server, it will have the same port across all computers
 * Also, on the same port as the root server, there will be a UDP Broadcast server that
 * will look for other servers on the network, and is responsible for populating the network graph
 */
typedef struct{
	p2pstate *state;
	int port;

	P2PRECEIVER code;

	pthread_t thread;
	int sock;

	bool running;

	/* Add stuff here as needed by the implementation of the p2pserv functions */

} p2pserver;

/**
 *  Initializes a p2pserver instance
 *
 *  @param state the global network state
 *  @param serv an unitialized server struct
 *  @param rcv code to execute when receiving a message
 *  @return zero on success
 */
												/* The port will be determined by matching to the pid in the app descriptor */
int p2pserv_init(p2pserver *serv, p2pstate *state, /*int port,*/ P2PRECEIVER rcv);

/**
 * Starts the listening server in a new thread under the calling process
 */
int p2pserv_start(p2pserver *serv);
int p2pserv_stop(p2pserver *serv);
int p2pserv_clean(p2pserver *serv);

typedef struct {
	p2pstate *state;

	/* Add stuff here as needed by the implementation of the p2pclient functions */
} p2pclient;


int p2pclient_init(p2pclient *client, p2pstate *state);
//int p2pclient_connect(p2pnode *node);
int p2pclient_send(p2pclient *c, p2pnode *node, char *msg, int lenmsg, char **response, int *lenresp);
int p2pclient_clean(p2pclient *c);


#endif /* P2PNET_H_ */
