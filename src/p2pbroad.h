
#include "p2pframe.h"

#include "p2pproto.h"
#include "p2pnet.h"


typedef struct {
	//int sock;
	p2pstate *state;


	p2pserver serv;
} p2pbc;

void p2pbroad_send(p2pbc *bc);
int p2pbroad_init(p2pbc *bc, p2pstate *s, P2PRECEIVER rcv);
int p2pbroad_start(p2pbc *bc);
