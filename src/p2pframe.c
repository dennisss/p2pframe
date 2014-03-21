//
//  main.c
//  p2p-frame
//
//  Created by Priyal Seebadri on 3/19/14.
//  Copyright (c) 2014 Priyal Seebadri. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>

/*
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <arpa/inet.h>
*/
#include "p2pnet.h"
#include "p2pframe.h"
//#include <ncurses.h>
#include "p2pbroad.h"

#include <unistd.h>

/* For adding connection */
#include <netinet/in.h>

p2pstate *state;
p2pserver rootserver;
p2pclient rootclient;
p2pbc broadcaster;

int size = 10;
int defaultPortNumber = 8088;
char path[1024] = "/bin/\0";
void startBroadcasting();
char* appNames[100];
void displayHostInformation();
void handleResponses();
char * getLocalIP();
void listNodes();
char ip[17];
int port = 0;
int host = 0;

void listNodes();
void listNodesMenu();
DIR *getDirectoryFromString(char* dir);
void getNewFolder();
int isDir(const char* path);
void printApplicationOptions(int *position, char * directory);
void setLocalName();
void root_receiver(char *msg, int lenmsg, int type, int sender);
void exitGracefully();
void joinConnection();
void runApps();
void printWelcomeLine();
int printBasicOptions();

void startapp(char *path, bool host, int connection)
{

	pid_t pid;

	if((pid = fork()) == NULL){ /* Child */

		/* Create the app and populate it with connections if needed */
		int app = p2pstate_newapp(state, "Unnamed");


		if(host){

		}
		else{
			p2pstate_addconnection(state, app, connection);
		}

		printf("Starting %s\n", path);

		//execve(path, path /* TODO: Change this */);
		execl(path, path, 0);


		printf("Failed to load application\n");

		exit(1);

	}
	else{
		/* Parent */
		int status;
		waitpid(pid, &status, WUNTRACED);


		/* Clean up the app entry here */
	}


}



int main(int argc, const char * argv[])
{
	if(p2pinit(&state, true) != 0){
		printf("Failed to initialize p2p state\n");
		exit(1);
	}

	int app = p2pstate_newapp(state, "p2pframework");


	if(p2pserv_init(&rootserver, state, root_receiver) != 0){
		printf("Failed to initialize p2p server\n");
		exit(1);
	}

	if(p2pserv_start(&rootserver) != 0){
		printf("Failed to start p2p server\n");
		exit(1);

	}


	setLocalName();
	startBroadcasting();

    printWelcomeLine();
    handleResponses();

    // insert code here...
    return 0;
}

void handleResponses()
{
    int response = printBasicOptions();
    switch( response )
    {
        case 1:
	    host = 1;
            runApps();
            break;
        case 2:
   	    host = 0;	
            joinConnection();
            break;
	case 3:
	    listNodesMenu();
        case 4:
            exitGracefully();
            break;
    }
}
void startBroadcasting()
{
	p2pbroad_init(&broadcaster, state, root_receiver);
	p2pbroad_start(&broadcaster);
	p2pbroad_send(&broadcaster);
}

int isDir(const char* path)
{
    struct stat s;
    if( stat(path, &s) == 0 )
    {
        //  printf("%i\n", S_IFDIR);
        if( s.st_mode & S_IFDIR )
        {
            return 1;
        }
    }
    return 0;
    
}

void listNodesMenu()
{
	listNodes();
	handleResponses();
}
void listNodes()
{
	int i;
	for(i = 0; i < state->nnodes; i++){
		//in_addr addr = state->nodes[i].gateway.s_addr;
		printf("%d:   %s %s\n", i,  state->nodes[i].name, inet_ntoa(state->nodes[i].gateway.s_addr));
		//printf("%d:   %s %s\n", i, state->nodes[i].name, inet_ntoa(state->nodes[i].gateway.sin_addr));
	}
}

void setLocalName()
{
	printf("Please enter a name for your machine: ");
	fflush(stdin);
	fgets(state->self.name, 128, stdin);

	if(state->self.name[strlen(state->self.name) - 1] == '\n')
		state->self.name[strlen(state->self.name) - 1] = 0;


	if(p2pclient_init(&rootclient, state) != 0){
		printf("Failed to initialize p2p client\n");
		exit(1);
	}
}

void printWelcomeLine()
{
    printf("Welcome to P2PFrame!\n");
}

int printBasicOptions()
{
    printf("Would you like to:\n1.) Host\n2.) Connect\n3.) List Nodes\n4.) Exit\n> ");
    char response[2];
    scanf("%1s", response);
    int resp = atoi(response);
    if (resp > 4 || resp < 1)
    {
        printf("Please choose one of the provided options.\n");
        return printBasicOptions();
    }
    return resp;
}

void connectApp(char * applicationPath)
{
	if(host)
		ip[strlen(ip) - 1] = 0;

		int nid = NULL;
		bool host = true;

		/* Give the client a preset server connection */
		if(strlen(ip) > 0){

			p2pnode nc;

			nid = atoi(ip);
			if(nid < 0 | nid >= state->nnodes){
				printf("Invalid node number, type 'list' to see all available ones\n");
			}

			host = false;
		}


		startapp(applicationPath, host, nid);
}
void runApps()
{
    char wantedDirectory[1024];
    getcwd(wantedDirectory, sizeof(wantedDirectory)); //get current working directory
    strcat(wantedDirectory, path); //And check in the folder designated by path
    printf("Checking for files in ./Apps directory\n");
    int *i = malloc(sizeof(int));
    *i = 1;
    printf("Please select which app you would like to run:\n");
    printApplicationOptions(i, wantedDirectory);
    int *applicationNumber = malloc(sizeof(int));
    *applicationNumber = 1;
    scanf("%d",applicationNumber, stdin);
    char * currentApplication = appNames[(*applicationNumber) - 1];
    connectApp(currentApplication);	
    free(i);
    free(applicationNumber);
}


/*
void getNewFolder()
{
    printf("Please enter the folder where compatible apps are stored\n");
    scanf("%1024s", path);
}
 */
void printApplicationOptions(int *position, char * directory)
{
    DIR *baseDirectory = getDirectoryFromString((directory));
    if (baseDirectory == 0)
    {
        printf("Error - The path %s does not point to a directory.\nPlease place all apps in the ./Apps directory\n",directory);
      //  getNewFolder();
        exitGracefully();
        return;
    }
     
    struct dirent *dirent = NULL;
    while (0 != (dirent= readdir(baseDirectory))) {
        char *sourceFilePath = (char *) malloc(1024);
        strcpy(sourceFilePath, directory);
        strcat(sourceFilePath, dirent->d_name);
        sourceFilePath[strlen(sourceFilePath)] = '\0';
        struct stat statbuf;
        stat(sourceFilePath, &statbuf);
        if (dirent->d_name[0] != '.'){
            if(isDir(sourceFilePath)) //If the current file is a directory that is not "." or "..".
            {
                strcat(sourceFilePath, "/");
                printApplicationOptions(position, sourceFilePath);
            }
            else //If it's not a directory, check for the substring
            {
                printf("%i.) %s\n",*position, dirent->d_name);
                char *appName = (char *) malloc(1024);
                strcpy(appName, sourceFilePath);
                appNames[(*position) - 1] = appName;
                (*position)++;

            }
            free(sourceFilePath);
        }
    }
}

void joinConnection()
{
    listNodes();
    printf("Please enter the node you would like to connect to:\n");
    scanf("%16s", ip);
    
    //Call the method for joining a host given an IP Address and Port
    
    runApps();
}
void exitGracefully()
{
    printf("Thanks for using P2PFrame!\n");
    exit(0);
}
    
DIR *getDirectoryFromString(char *dir)
{
        DIR *directory;
        if ((directory = opendir(dir)) == NULL)
        {
            fprintf(stderr, "Can't open %s\n", dir);
            return 0;
        }
        return directory;
}

void root_receiver(char *msg, int lenmsg, int type, int sender)
{

	/*
	P2P_MSG_HEARTBEAT 1
	P2P_MSG_FTP
	P2P_MSG_WHO
	P2P_MSG_ID
	P2P_MSG_ACK
	P2P_MSG_DATA
	 */

	if(type == P2P_MSG_WHO){
		/* Send back this computer's id */

		if(lenmsg != sizeof(p2pnode)){
			printf("bad node received\n");
		}

		p2pnode *n = (p2pnode *)msg;

		strcpy(&state->nodes[sender].name, &n->name);


		char *m = "hello";
		/* For now this will just send a blank message back to the other client so that it knows that we exist */
		p2pclient_send(&rootclient, &state->nodes[sender], m,strlen(m),0,0);



		/* Ignore messages sent by itself */
		/*if(uuidcmp(&n->uuid, &state->self.uuid) == 0){


		}*/

		///printf("got a who is\n");

	}

	if(type == P2P_MSG_ID){

		/* Store the ID of another server */

	}

	if(type == P2P_MSG_CONNECT){
		/* Do an acknowledgment on the external client's request to connect */
			/* Fill the connections array appropriately */
			/* Send over any needed files */
	}

	if(type == P2P_MSG_QUERY){
		/* Return what apps the server is running and other neat states maybe... */

	}

	/* If a connect request message: */
		/* If the app is NOT started */
			/* Create new app and starts it  */

		/* App waits for connection request from the remote app */
		/* Apps keep going back and forth depending on what they are doing */

}

/*
char * getLocalIP()
{
    int fd;
    struct ifreq ifr;
    
    fd = socket(AF_INET, SOCK_DGRAM, 0);
    
    /* I want to get an IPv4 IP address 
    ifr.ifr_addr.sa_family = AF_INET;
    
    /* I want IP address attached to "eth0" 
    strncpy(ifr.ifr_name, "eth0", IFNAMSIZ-1);
    
    ioctl(fd, SIOCGIFADDR, &ifr);
    
    close(fd);
    
    /* display result 
    char * ip = inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr);
    return ip;

}
*/
