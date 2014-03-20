#include "tic.h"
#include <stdlib.h>
#include "../src/p2pnet.h"
#include "../src/p2pframe.h"
#include <arpa/inet.h>
#include <netinet/in.h>

p2pserver serv;
p2pclient client;
p2pstate *state;
int app;
int HOST = 1;
int CLIENT = 2;

int currentPlayer;
void gameLoop();
void receivemsg(char *msg, int lenmsg, int type, int sender);
bool connected = false;
char *temp_msg;
int senderNodeId = NULL;

void receivemsg(char *msg, int lenmsg, int type, int sender)
{
	printf("sender received - %d\n", sender);
	if (senderNodeId == NULL)
		senderNodeId = sender;

	printf("Recv: \n");
	temp_msg = msg;
	connected = true;

	if(strcmp(msg, "Quit") == 0){
		printf("Exit command...\n");
		exit(0);
	}
		printf("%s\n", msg);
}

int main(int argc, char *argv[])
{
	//Test the connections
	if(p2pinit(&state, false) != 0){
		printf("Failed to initialize p2p state\n");
		exit(1);
	}


	app = p2pstate_newapp(state, "Tic Tac Tow");

	if(p2pserv_init(&serv, state, receivemsg) != 0){
		printf("Failed to initialize p2p server\n");
		exit(1);
	}

	if(p2pserv_start(&serv) != 0){
		printf("Failed to start p2p server\n");
		exit(1);

	}


	if(p2pclient_init(&client, state) != 0){
		printf("Failed to initialize p2p client\n");
		exit(1);
	}


  	if(state->apps[app].nconnections > 0)
	{
   		 char *msg = "Start the Game";
   		 printf("Send?: %s\n", msg);
   		 p2pclient_send(&client, & state->nodes[state->apps[app].connections[0] ], msg, strlen(msg), NULL, NULL);
	  	currentPlayer = HOST;
			gameLoop();			
		}
 	 else{
 			printf("Waiting for a connection...\n");
  	  while(!connected){
   	   sleep(1);
  	  }
  	  printf("Got a connection.\n");
			currentPlayer = CLIENT;
			//p2pnode nc;
			//inet_aton(temp_msg, &nc.gateway.s_addr);
			 //int nid = p2pstate_addnode(state, &nc);
			 //int app = p2pstate_newapp(state, "Unnamed");
			 //p2pstate_addconnection(state, app, nid);
			gameLoop();
 	 }
	
}
void print_grid(char board[3][3])
{
	int i,j;
	printf("\n\n");
	for(i=0;i<3;i++)
	{
		for(j=0;j<3;j++)
		{
			if(board[i][j] == '\0')
			{
				printf("%2d ",3*(i)+(j+1));
			}
			else
			{
				printf(" %c ",board[i][j]);
			}
			if(j!=3)
			{
				printf("|");
			}
		}
		if(i != 3)
		{
			printf("\n-------------------\n");
		}
	}
}

int didwin(char board[3][3])
{
	int i,j;
	char current;
	char winner = '\0';

	//Iterate over rows to check for winner
	for(i=0;i<3;i++)
	{
		current = board[i][0];
		for(j=0;j<3;j++)
		{
			if(board[i][j] != current)
			{
				current = '\0';
			}
		}
		if(current != '\0')
		{
			winner = current;
		}
	}

	//Iterate over columns
	for(i =0;i<3;i++)
	{
		current = board[0][i];
		for(j=0;j<3;j++)
		{
			if(board[j][i] != current)
			{
				current = '\0';
			}
		}
		if(current != '\0')
		{
			winner = current;
		}
	}
	
	//Iterate over diagonals
	current = board[0][0];
	for(i=0;i<3;i++)
	{
		if(board[i][j] != current)
		{
			current = '\0';
		}
	}
	if(current != '\0')
	{
		winner = current;
	}

	current = board[0][3];
	for(i=0;i<3;i++)
	{
		if(board[i][3-i] != current)
		{
			current = '\0';
		}
	}
	if(current != '\0')
	{
		winner = current;
	}
	return winner;
}

void gameLoop()
{
	int i=0, j=0, lead=0, player=0, nrows=0, ncols=0, winner=0;
	char *msg = malloc(sizeof(char) * 8);
	char again='q';
	char four_X_four[3][3];

	while(winner == '\0')
	{
		//Setting board up
		for(i = 0; i < 3; i++)
		{
			for(j = 0; j < 3; j++)
			{
				four_X_four[i][j] = '\0';
			}
		}
		for(i = 0; i<9 && winner==0; i++)
		{

			/*
			printf("Debug net: %d %d\n", app, state->apps[app].connections[0]);
			int j;
			for(j = 0; j < state->nnodes; j++){
				printf("%d:   %s %s\n", j, state->nodes[j].name, inet_ntoa(state->nodes[j].gateway));
			}
			*/


			temp_msg=NULL;
			print_grid(four_X_four);
			player = i%2 +1;
			printf("Beginning of Loop %d\n", player);
			if(player==1 && currentPlayer == HOST)
			{
				printf("\n Player %d, please enter the number of the square where you want to place you %c: ",player,'X');
				scanf("%s",msg);
				//Send info out
				p2pclient_send(&client, &state->nodes[state->apps[app].connections[0]],msg, strlen(msg),NULL,NULL);
			}
			else if(player==2 && currentPlayer == CLIENT)
			{
				printf("\n Player %d, please enter the number of the square where you want to place %c: ",player,'O');
				scanf("%s",msg);
				printf("Sender ID = %d\n", senderNodeId);
				//Send info out
				p2pclient_send(&client,&state->nodes[senderNodeId],msg,strlen(msg),NULL,NULL);
			}
			else{
				printf("Waiting for Input");
				//Wait for stuff back
				while(temp_msg==NULL)
				{
					//printf(".");
				}
				printf("%s", temp_msg);
				msg=temp_msg;
			}
			//scanf("%d",&lead);
			lead = atoi(msg);
			printf("%d", lead);
			lead--;
			ncols = lead%3;
			lead = lead - ncols;
			nrows = lead/3;
			if(lead<0 || lead>9 || four_X_four[nrows][ncols]=='X' || four_X_four[nrows][ncols]=='O')
			{
				printf("Space is already taken, please try again");
				i--;
			}
			else
			{
				printf("Place X\n");
				four_X_four[nrows][ncols] = (player==1) ? 'X' : 'O';
			}
			printf("Before Winner\n");
			winner = didwin(four_X_four);
		}
		if(winner != '\0')
		{
			printf("Winner was %c! Good job.\n",winner);
			printf("Do you want to play again? y/n: ");
			while(!(again == 'y' || again == 'n'))
			{
				scanf("%c", &again);
			}
			if(again == 'y')
			{
				winner = '\0';
				again = 'q';
			}
		}
		else
		{
			printf("No winner this round. Try again.");
		}
		printf("End of loop\n");
	}
	p2pclient_clean(&client);
	p2pserv_stop(&serv);
	p2pserv_clean(&serv);
	return;

}

