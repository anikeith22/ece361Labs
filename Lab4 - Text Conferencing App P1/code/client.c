#include <stdio.h> 
#include <stdlib.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>  
#include <stdbool.h>
#include "globals.h"
#include "packet.h"
// #include "session.h"
// #include "accounts.h" 
// #include "globals.h"
// completed by anikeith bankar 1004073478 and jay bihola 1004178310

//commands for this section of Lab 
char * LOGIN_CMD = "/login";
char * CREATE_SESSION = "/createsession";
char * LIST = "/list"; 
char * QUIT = "/quit";

int main(int argc, char *argv[]) {
	char type_of_command[20] = {0}; 
	char clientID [20] = {0};
	char password [20] = {0};
	char IP_address [20] = {0};
	char port[20] = {0}; 
	char sessionID [200] = {0};  
	char server_response [20000]; 
	char server_response2 [20000];
	char send_to_server[2000];  
	bool quit_process = false; 
	int network_sock;  
	int connection_status;

	// the above allocations of arrays are for testing purposes will change later to the global variables
	printf("Welcome to Text Conferencing APP : J & A : Version 2019\n");  

	while (!quit_process) { 
		

		scanf("%s", type_of_command);  
		printf("%s\n", type_of_command);
		if (strcmp(type_of_command, "/login") == 0) { 

			scanf("%s", clientID);
			scanf("%s", password);
			scanf("%s", IP_address);
			scanf("%s", port);

			printf("%s %s %s %s \n", clientID, password, IP_address, port);


			printf("i am in if\n"); 
			printf("Logging in ...\n");
			printf("port number %s\n", port);
			int port_no = atoi(port);  



			// create a socket 
			if ((network_sock = socket(AF_INET, SOCK_STREAM, 0))<0) {
				printf("error in socket\n");
			}



			//specify an address for the socket 
			struct sockaddr_in server_address; 
			server_address.sin_family = AF_INET;
			server_address.sin_port = htons(port_no);
			server_address.sin_addr.s_addr = INADDR_ANY; 

			inet_pton(AF_INET, IP_address, &server_address.sin_addr);

			connection_status = connect(network_sock, (struct sockaddr *) &server_address, sizeof(server_address)); 
			


			//check for error with the connection
			if (connection_status == -1) {
				printf ("There was an error making a connection to the remote socket \n\n");
			}


			// make the above info given into a packet
			struct message * login_packet = (struct message *)malloc(sizeof(struct message)); 
			login_packet->type = LOGIN;
			login_packet->size = strlen(clientID)+strlen(password); 
			strcpy(login_packet->source, clientID); 
			strcat(clientID,",");
			strcat(clientID, password); 
			strcpy(login_packet->data, clientID);


			// use the packet to string function from packet.h
			
			decode_message(login_packet, send_to_server);


			// send that string over to the server  
			send(network_sock, send_to_server, sizeof(send_to_server),0);				


			// recieve data from the server 
			read(network_sock, server_response, sizeof(server_response));  



			// we need to use the string to packet function and see what the message type is
			struct message * response_server = string_to_message(server_response);
			
			// if message type is LO_ACK then it was successful login 
			if (response_server->type == LO_ACK) printf("\nSuccessful Login"); 
		
			// if message type is LO_NAK then it was unsuccessful and we have to print out packet->data which 
			// would be the error message			
			else if (response_server->type == LO_NAK) printf("%s",response_server->data);	 

			//server_response = 0;		
			//send_to_server = 0;
			free(login_packet);		 
			free(response_server);
		
		}  

		else if (strcmp(type_of_command, CREATE_SESSION) == 0) { 
			scanf("%s", sessionID);
			printf("Connecting to session ...\n");	 


			// use the packet to string function to send over NEW_SESS message type to server
			struct message * create_session = (struct message *)malloc(sizeof(struct message));
			create_session->type = NEW_SESS; 
			create_session->size = strlen(sessionID);
			strcpy(create_session->source, clientID);
			strcpy(create_session->data, sessionID); 


			//send_to_server = 0;
			//server_response = 0;
			
			// use the packet to string function from packet.h			
			decode_message(create_session, send_to_server);


			// send that string over to the server  
			send(network_sock, send_to_server, sizeof(send_to_server),0);			


			// server sends back a NEW_SESS message type back 
			read(network_sock, server_response, sizeof(server_response));  


			printf("server response: %s \n", server_response);
			// we need to use the string to packet function and see what the message type is
			struct message * response_server = string_to_message(server_response);
			if(response_server->type == NS_ACK) 
				printf("You have successfully created session: %s and joined it.",response_server->data); 

			free(create_session);
			free(response_server);
		}
		

		else if (strcmp(type_of_command, LIST) == 0) {
			struct message * list = (struct message *)malloc(sizeof(struct message));
			list->type = QUERY;
			list->size = 0;
			strcpy(list->source, clientID); 

			//send_to_server = 0;
			//server_response = 0;

			// use the packet to string function from packet.h			
			decode_message(list, send_to_server);
			
			// send that string over to the server  
			send(network_sock, send_to_server, sizeof(send_to_server),0);			

			// server sends back a NEW_SESS message type back 
			recv(network_sock, &server_response, sizeof(server_response), 0);
			
			// we need to use the string to packet function and see what the message type is
			struct message * response_server = string_to_message(server_response);

			if(response_server->type == QU_ACK) 
				printf("The list of online users and available sessions:\n %s",response_server->data); 

			free(list);
			free(response_server);
			 
		} 
	
		else if (strcmp(type_of_command, QUIT) == 0) {
			
			struct message * quit = (struct message *)malloc(sizeof(struct message));
			quit->type = EXIT;
			quit->size = 0;
			strcpy(quit->source, clientID); 
			//list->data = 0; 

			//send_to_server = 0;
			//server_response = 0;

			// use the packet to string function from packet.h			
			decode_message(quit, send_to_server);
			
			// send that string over to the server  
			send(network_sock, send_to_server, sizeof(send_to_server), 0);			

			// server sends back a NEW_SESS message type back 
			recv(network_sock, &server_response, sizeof(server_response), 0);
			
			// we need to use the string to packet function and see what the message type is
			struct message * response_server = string_to_message(server_response);

			if(response_server->type == QU_ACK) 
				printf("Successfuly Exiting Server!"); 

			free(quit);
			free(response_server);
			quit_process = true; 

		}  
	} 
	// and then close the socket 
	close(network_sock);
	return 0; 
}

