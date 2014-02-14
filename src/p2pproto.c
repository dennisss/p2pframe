#include "p2pproto.h"

char P2PF_MAGIC[] = {'P', '2', 'P', 'F'};

int p2phdr_create(p2pheader *hdr, int class, int type, p2pnode *dst, int length)
{
	hdr->class = class;
	memcpy(&hdr->destination, &dst->uuid, sizeof(uuid_t));
	hdr->length = length;
	memcpy(&hdr->magic, P2PF_MAGIC, 4);
	hdr->type = type;
}

