/* This file describes the specific details of the data transfer formats/protocol  */
#ifndef P2PPROTO_H_
#define P2PPROTO_H_

/* Only put functions for creating and reading messages here */

#include "p2pframe.h"

#define P2P_MSG_HEARTBEAT 1 /* Keep alive */
#define P2P_MSG_FTP 2 /* Transferring trusted and untrusted files around to the computer or to the client's ~/.p2p */
#define P2P_MSG_WHO 3 /* Typically broadcasted in search of other computers on the net. Others respond with their ID  */
#define P2P_MSG_ID 4 /* Used to send the keys/identification info back to another computer */
#define P2P_MSG_ACK 5 /* Acknowledge message sent back by the server for every received request that does not have data associated with the response */
#define P2P_MSG_DATA 6 /* For generic data transfers */


typedef struct {
	char magic[4];
	int32_t length; /* Size of payload data */

	int32_t class; /* For distinguishing between framework and application messages? */
	int32_t type;

	/*
	 * This is to make sure that things get routed properly through complex networks.
	 * Each node in the local network list will have associated gateway information.
	 * The local node will send the message along that gateway node. The receiving node
	 * will check the destination of the message. If it is for them, they process the message,
	 * otherwise, they pass the message in a recursive manner
	 *
	 * The gateway information will also need to have "num of steps" or latency information for
	 * picking the best routes
	 *
	 * A completely null address will indicate a broadcast message
	 * */
	uuid_t destination;

} p2pheader;


/* This struct represents the actual structure of the data that will be sent over the net */
typedef struct {
	p2pheader header;
	char payload[];
} p2pmsg;

/* Populates an p2pheader structure with data so that it is ready to send as a header before any data */
int p2phdr_create(p2pheader *hdr, int class, int type, p2pnode *dst, int length);


#endif /* P2PPROTO_H_ */
