/* Functions to create, manipulate, and query the p2pstate */


#include "p2pframe.h"
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>

/* Used for identifying shared process memory and also for storing the identity of the computer */
/* TODO: In the future, this file can also serve as a lock against multiple instances of the framework */
char *ID_FILE = "p2p.id";
int PROJ_ID = 1234;

int p2pinit(p2pstate **state, bool new)
{
	/* Make sure that the file exists */
	FILE *file = fopen(ID_FILE, "a+");

	if(file == NULL){
		/* An error occured in opening the file */
		printf("Failed to open file\n");
	}

	/* Generate a common key to identify the shared memory based on the file */
	key_t k = ftok(ID_FILE, PROJ_ID);

	int shmid;

	shmid = shmget(k, sizeof(p2pstate), IPC_CREAT | 0666 /* TODO: Make sure that it is only created for new framework instances */);

	if(shmid < 0){
		printf("shmget failed\n");
		return 1;
	}

	*state = shmat(shmid, NULL, NULL /* TODO: Check this parameter */);

	if(*state == -1){
		printf("Failed to attach memory %d\n", shmid);
		return 2;
	}


	if(new){
		/* Begin initializing the state ONLY IF it is a new one */

		fseek(file, 0, SEEK_END);

		/* Establish self identity */
		if(ftell(file) != 0){ /* Data already exists in file, rewind and read identity */
			rewind(file);
			if(fread(&(*state)->self, sizeof(p2pnode), 1, file) != sizeof(p2pnode)){
				/* Failed to read entire identity. Corrupt? Maybe regenerate it */
			}
		}
		else{ /* Create a new identity and write it to the file */
			inet_aton("127.0.0.1", &(*state)->self.gateway.s_addr);
			strcpy((*state)->self.name, "Some name from the user"); //TODO: Have the name as a parameter or something
			generateuuid(&(*state)->self.uuid);
			fwrite(&(*state)->self, sizeof(p2pnode), 1, file);
		}


		(*state)->napps = 0;
		(*state)->nnodes = 0;

	}

	fclose(file);

	return 0;
}


/* Simple hashing function for converting an app name to a port number */
int hashname(char *name)
{
	int val = 0;
	int i = 1;
	while(*name != 0){
		val += (*name) * i;
		i++;
	}

	return val;
}

int p2pstate_newapp(p2pstate *s, char *name)
{
	int i = s->napps++;

	strcpy(&s->apps[i].name, name);

	s->apps[i].nconnections = 0;
	s->apps[i].pid = getpid();

	if(i == 0) /* The first app will always be the root server */
		s->apps[i].port = ROOT_PORT;
	else
		s->apps[i].port = 2048 + (hashname(name) % 20000); /* Get a port in the range 2048 - 22048 (this port range has no particular meaning) */

	return i;
}


p2papp* p2pstate_getapp(p2pstate *s)
{
	int x;
	pid_t thread = getpid();
	for(x = 0; x < s->napps; x++){
		if(s->apps[x].pid == thread)
			return &s->apps[x];
	}

	return NULL;
}


void generateuuid(uuid_t *buffer)
{
	int x;
	for(x = 0; x < sizeof(uuid_t); x++){
		buffer->vals[x] = (char)rand();
	}

}
