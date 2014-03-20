
#include "p2pframe.h"

#include "p2pproto.h"
#include "p2pnet.h"


typedef struct {
	//int sock;
	p2pstate *state;


	p2pserver serv;
} p2pbc;
