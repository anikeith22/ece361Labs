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
#include <sys/time.h>
#include <sys/poll.h>


// completed by anikeith bankar 1004073478 and jay bihola 1004178310

//commands for this section of Lab 
char * LOGIN_CMD = "/login";
char * CREATE_SESSION = "/createsession";
char * LIST = "/list"; 
char * QUIT = "/quit";
char * LOGOUT = "/logout";
char * JOIN_SESSION = "/joinsession";
char * LEAVE_SESSION = "/leavesession";
char * REGISTER_USER = "/register";

int main(int argc, char *argv[]) {
	char type_of_command[20] = {0}; 
	char clientID [50] = {0};
    char login[50];
	char password [50] = {0};
	char IP_address [20] = {0};
	char port[20] = {0}; 
	char sessionID [200] = {0};  
	char server_response [20000]; 
	char server_response2 [20000];
	char send_to_server[2000];  
	bool quit_process = false; 
	int network_sock = 0;  
	int connection_status;
	bool loggedIn; 
	char messageBuffer[20000];
    
	struct timeval timeout;  
	struct timeval starting, ending; 
	timeout.tv_sec = 2; 
	timeout.tv_usec = 0;  

	clock_t start, end; 

	fd_set read_fd, input_fd;  

	bool endthis = false; 
	bool execute_command = false; 
	bool inactive = false;  
	int difference; 
	int timer = 0; 

	// the above allocations of arrays are for testing purposes will change later to the global variables
	
	printf("Welcome to Text Conferencing APP : AJ : Version 2019\n");  

	while (!quit_process) {  
		 
		while (loggedIn && !execute_command) {   

			FD_ZERO(&read_fd);
			FD_SET(network_sock, &input_fd);

			// checking the socket 
			int reading_socket = select (network_sock + 1, &input_fd, NULL, NULL, &timeout); 

			if (reading_socket == 0) break; 

			else if (reading_socket < 0) { 
				printf("Error reading socket\n"); exit(EXIT_FAILURE);
			}
			
			else {
				read(network_sock, messageBuffer, sizeof(messageBuffer));
				struct message * message = string_to_message(messageBuffer);
				if (message->type == MESSAGE) { 
					printf("%s\n", message->data); 
				}
				free(message);
                break;
			}
            // strcpy(type_of_command, "blank");
		}

		execute_command = false;

		FD_ZERO(&input_fd);
                FD_SET(0, &input_fd); 
                    
		timer = timer + 1;   

		if(timer > 60000000 && loggedIn) { // 60 million is for 1 minute. 
			printf("\nYou were inactive for way too long sorry, keep up!\n");
			timer = 0; 
			inactive = true; 
		}

		if (select(0 + 1, &input_fd, NULL, NULL, &timeout) < 0){
            	exit(EXIT_FAILURE); 
        }

		if (FD_ISSET(0, &input_fd)) {      
			
			//scanf("%s", type_of_command);
			
			if (!inactive) {  
				scanf("%s", type_of_command);
				//printf("%d\n", timer);
				timer = 0; 
			} else {
				//scanf("%s", type_of_command); 
				//printf("%d\n", timer);
				strcpy(type_of_command, QUIT); 
			}

			if (strcmp(type_of_command, LOGIN_CMD) == 0) { 
				scanf("%s", clientID);
				scanf("%s", password);
				scanf("%s", IP_address);
				scanf("%s", port);
							
				int port_no = atoi(port);  

				// create a socket 
				if ((network_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
					printf("Error in socket\n");
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
					printf ("\nThere was an error making a connection to the remote socket \n\n");
				}

				// make the above info given into a packet
				struct message * login_packet = (struct message *)malloc(sizeof(struct message)); 
				login_packet->type = LOGIN;
				login_packet->size = strlen(clientID)+strlen(password); 
				strcpy(login_packet->source, clientID);
							strcpy(login, clientID);
				strcat(login,",");
				strcat(login, password);
				strcpy(login_packet->data, login);

				// use the packet to string function from packet.h
				decode_message(login_packet, send_to_server);

				// send that string over to the server  
				send(network_sock, send_to_server, sizeof(send_to_server),0);				

				// recieve data from the server 
				read(network_sock, server_response, sizeof(server_response));  

				// we need to use the string to packet function and see what the message type is
				struct message * response_server = string_to_message(server_response);
				
				// if message type is LO_ACK then it was successful login 
				if (response_server->type == LO_ACK) { 
					printf("\nSuccessful Login\n\n"); 
					loggedIn = true;
					execute_command = true;
				}  
			
				// if message type is LO_NAK then it was unsuccessful and we have to print out packet->data which 
				// would be the error message			
				else if (response_server->type == LO_NAK) printf("\n%s\n",response_server->data);	 

				free(login_packet);		 
				free(response_server);
			}  

			else if (strcmp(type_of_command, CREATE_SESSION) == 0) { 
				execute_command = true;

				scanf("%s", sessionID);
							printf("\nConnecting to session ...\n\n");   

							// use the packet to string function to send over NEW_SESS message type to server
							struct message * create_session = (struct message *)malloc(sizeof(struct message));
							create_session->type = NEW_SESS; 
							create_session->size = strlen(sessionID);
							strcpy(create_session->source, clientID);
							strcpy(create_session->data, sessionID); 

							// use the packet to string function from packet.h          
							decode_message(create_session, send_to_server);


							// send that string over to the server  
							send(network_sock, send_to_server, sizeof(send_to_server),0);           


							// server sends back a NEW_SESS message type back 
							read(network_sock, server_response, sizeof(server_response));  


							//printf("\nserver response: %s \n\n", server_response);

							// we need to use the string to packet function and see what the message type is
							struct message * response_server = string_to_message(server_response);
							if(response_server->type == NS_ACK) { 
								printf("\nYou have successfully created session: %s and joined it.\n\n",response_server->data); 
							
							}

							free(create_session);
							free(response_server);
			}  
			else if (strcmp(type_of_command, JOIN_SESSION) == 0) { 
										execute_command = true;
				scanf("%s", sessionID);
							printf("\nJoining to session ...\n\n");
							// use the packet to string function to send over NEW_SESS message type to server
							struct message * join_session = (struct message *)malloc(sizeof(struct message));
							join_session->type = JOIN;
							join_session->size = strlen(sessionID);
							strcpy(join_session->source, clientID);
							strcpy(join_session->data, sessionID); 

							// use the packet to string function from packet.h          
							decode_message(join_session, send_to_server);
							// send that string over to the server  
							send(network_sock, send_to_server, sizeof(send_to_server),0);           

							// server sends back a NEW_SESS message type back 
							read(network_sock, server_response, sizeof(server_response));  

							char buffer[3000]; 
							char *session_id;
							char *reason_error; 
							char *token;
							struct message * response_server = string_to_message(server_response);
							printf("data: %s\n", response_server->data);
							strcpy(buffer, response_server->data); 

							token = strtok(buffer,",");
							session_id = token;

							// we need to use the string to packet function and see what the message type is
							if(response_server->type == JN_ACK) {
								printf("\nYou have successfully joined session: %s.\n\n", session_id);   
							} else if (response_server->type == JN_NACK) {
								token = strtok(NULL, ",");
								reason_error = token;
								printf("\nYou are have not joined the session: %s, because: %s\n\n", session_id, reason_error);
							}
							free(join_session);
							free(response_server);
			
			} else if (strcmp(type_of_command, LEAVE_SESSION) == 0) { 
										execute_command = true;
				printf("Leaving session ...\n");	 

				// use the packet to string function to send over NEW_SESS message type to server
				struct message * leave_session = (struct message *)malloc(sizeof(struct message));
				leave_session->type = LEAVE_SESS; 
				leave_session->size = strlen(sessionID);
				strcpy(leave_session->source, clientID);
				
				// use the packet to string function from packet.h			
				decode_message(leave_session, send_to_server);

				// send that string over to the server  
				send(network_sock, send_to_server, sizeof(send_to_server),0);			

				// server sends back a NEW_SESS message type back 
				read(network_sock, server_response, sizeof(server_response));  
				
				// we need to use the string to packet function and see what the message type is
				struct message * response_server = string_to_message(server_response);
				
				if(response_server->type == LEAVE_ACK) {
					printf("\nYou have successfully left session: %s.\n\n", response_server->data);   
					
				}
				free(leave_session);
				free(response_server);
			}

			else if (strcmp(type_of_command, LIST) == 0) {
										execute_command = true;
							struct message * list = (struct message *)malloc(sizeof(struct message));
							list->type = QUERY;
							list->size = 0;
							strcpy(list->source, clientID); 
							memset(send_to_server, 0, sizeof(send_to_server));

							// use the packet to string function from packet.h          
							decode_message(list, send_to_server);

							// send that string over to the server  
							send(network_sock, send_to_server, sizeof(send_to_server),0);           

							// server sends back a NEW_SESS message type back 
							recv(network_sock, &server_response, sizeof(server_response), 0);

							// we need to use the string to packet function and see what the message type is
							struct message * response_server = string_to_message(server_response);

							if(response_server->type == QU_ACK) {
								printf("\nThe list of online users and available sessions:\n %s\n",response_server->data); 
							}
								
							free(list);
							free(response_server);
				
			}
			else if (strcmp(type_of_command, QUIT) == 0) { 


				if (!loggedIn) {
					quit_process = true;
					break;
				}
				
				execute_command = true;
				struct message * quit = (struct message *)malloc(sizeof(struct message));
				quit->type = EXIT;
				quit->size = 0;
				strcpy(quit->source, clientID); 
				strcpy(clientID, "\0");
				// use the packet to string function from packet.h			
				decode_message(quit, send_to_server);
				
				// send that string over to the server  
				send(network_sock, send_to_server, sizeof(send_to_server), 0);		 
				// server sends back a NEW_SESS message type back 

				recv(network_sock, &server_response, sizeof(server_response), 0);

				// we need to use the string to packet function and see what the message type is
				struct message * response_server = string_to_message(server_response);

				if(response_server->type == EXIT_ACK) {
					printf("\n%s\n\n",response_server->data); 
				}

				free(quit);
				free(response_server); 
				quit_process = true; 
				// break; 
			
			} else if (strcmp(type_of_command, LOGOUT) == 0) {
				
				execute_command = true;
				struct message * logout = (struct message *)malloc(sizeof(struct message));
				logout->type = EXIT;
				logout->size = 0;
				strcpy(logout->source, clientID); 
				strcpy(clientID, "\0");
				// use the packet to string function from packet.h			
				decode_message(logout, send_to_server);
				
				// send that string over to the server  
				send(network_sock, send_to_server, sizeof(send_to_server), 0);			

				// server sends back a EXIT_ACK message type back 
				recv(network_sock, &server_response, sizeof(server_response), 0);
				
				// we need to use the string to packet function and see what the message type is
				struct message * response_server = string_to_message(server_response);

			    if(response_server->type == EXIT_ACK) { 
					printf("\n%s\n\n", response_server->data);  
					loggedIn = false;  
				}

				free(logout);
				free(response_server);
			
			}	else if (strcmp(type_of_command, REGISTER_USER) == 0) {
						execute_command = true;
						scanf("%s", clientID);
						scanf("%s", password);
						scanf("%s", IP_address);
						scanf("%s", port);
						printf("%s %s %s %s \n", clientID, password, IP_address, port);

						int port_no = atoi(port);

						// create a socket
						if ((network_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
							printf("\nError in socket\n\n");
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
						login_packet->type = REGISTER;
						login_packet->size = strlen(clientID)+strlen(password);
						strcpy(login_packet->source, clientID);
						strcat(login,clientID);
						strcat(login,",");
						strcat(login, password);
						strcpy(login_packet->data, login);

						// use the packet to string function from packet.h
						decode_message(login_packet, send_to_server);

						// send that string over to the server
						send(network_sock, send_to_server, sizeof(send_to_server),0);
						

						// recieve data from the server
						read(network_sock, server_response, sizeof(server_response));

						// we need to use the string to packet function and see what the message type is
						struct message * response_server = string_to_message(server_response);

						// if message type is LO_ACK then it was successful login
						if (response_server->type == REGISTER_ACK) { printf("*****\nSuccessful Registration\n*****\n"), loggedIn = true; }

						// if message type is LO_NAK then it was unsuccessful and we have to print out packet->data which
						// would be the error message
						else if (response_server->type == REGISTER_NAK) printf("%s",response_server->data);
						free(login_packet);
						free(response_server);
			//			
				} else  { // send a message to the server, only works when logged in 
						execute_command = false;
						char message_buffer[2000];
						memset(message_buffer, 0, sizeof(message_buffer));
						fgets(message_buffer, sizeof(message_buffer), stdin);
						strcat(type_of_command, message_buffer); 

						memset(send_to_server, 0, sizeof(send_to_server));
						
						struct message * message_to_send = (struct message *)malloc(sizeof(struct message));
						message_to_send->type = MESSAGE;
						message_to_send->size = strlen(type_of_command);
						strcpy(message_to_send->source, clientID);
						strcpy(message_to_send->data, type_of_command); 

						// use the packet to string function from packet.h			
						decode_message(message_to_send, send_to_server);
				
						// send that string over to the server  
						send(network_sock, send_to_server, sizeof(send_to_server), 0);			

						// server sends back a MESSAGE_ACK message type back 
						recv(network_sock, &server_response, sizeof(server_response), 0);
				
						// we need to use the string to packet function and see what the message type is
						struct message * response_server = string_to_message(server_response);

						if(response_server->type == MESSAGE_ACK) {
							printf("\n Successfully sent message to the chat!\n\n");
						}
						free(message_to_send);
						free(response_server); 
				} 
		}
	}

	printf("You have left the text conferencing AJ APP\nBye For Now!\n");
	
        // and then close the socket 
	close(network_sock); 
	exit(EXIT_SUCCESS);
	return 0; 
}

