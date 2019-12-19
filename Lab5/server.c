#include "globals.h"
#include "session.h"
#include "account.h"
#include "packet.h" 
#include "stdlib.h"

#define MAXCLIENTS 5

// completed by anikeith bankar 1004073478 and jay bihola 1004178310

struct session_struct *session_list[max_sessions];
char dir[100] = "accounts.txt";

int session_to_join(char name[]) {
  for (int i = 0; i < max_sessions; i++) {
    if (session_list[i] != NULL) {
      if (strcmp((session_list[i])->session_name,name)==0)
        return i;
    }
  }
  return -1;
}
 
int main (int argc, char *argv[]) {

  struct account_struct *account_list = (struct account_struct*)malloc(sizeof(struct account_struct)); 

  // file decriptors 
  fd_set read_fd; 
  int max_sd, sd;  
  int client_list[5] = {0}; 
	char server_message [2048]; 
	char client_response [2000]; 
	//create the server socket 
	int server_socket;  
	
	int opt = 1;
    account_list = create_list();
    update_list(account_list);
//  	new_account_char(&account_list, "jay", "test");
//  	new_account_char(&account_list, "anikeith", "test");

   open("output", O_CREAT|O_WRONLY, 0777); 


    int session_to_create = 0;
    
    if (argc != 2) { // check if there's 2 arguments given, if not ...
        printf("Error 1:\n Invalid Usage\n Usage Format: server (TCP listen port), i.e., server 2000\n");
    }

	server_socket = socket(AF_INET, SOCK_STREAM, 0); // TCP socket using SOCK_STREAM and 0
	
	setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	
	if (server_socket < 0) {
       	printf("Error 2:\n TCP Socket Connection Error! Unable to establish connection with socket\n");
        return 0;
  	}
	  	

	int port = atoi(argv[1]);
	//define the server address 
	struct sockaddr_in server_address; 
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(port);
	server_address.sin_addr.s_addr = htonl(INADDR_ANY); 

	//bind the socket to our specified IP and port 
	if (bind(server_socket, (struct sockaddr*) &server_address, sizeof(server_address))<0) {
		printf("bind failed\n"); 
		exit(EXIT_FAILURE);
	}
	
	listen(server_socket, 7);  

  	// variables to be used in loop
  	char session_name[30];
  	char cID[cID_len];  
	char cPW[cPW_len]; 
	struct message * message_from_client; 

	int client_socket; 

	char error_message [100] ; 
	
	while(true) {
        
        printf("...\n");
		
        FD_ZERO(&read_fd);
        FD_SET(server_socket, &read_fd);
        max_sd = server_socket;
		
		
        for (int i = 0; i < MAXCLIENTS; ++i){
            sd = client_list[i];
            if (sd > 0) {
				FD_SET(sd, &read_fd);
			}
            if (sd > max_sd) {
				max_sd = sd;
			}
        }
        

        //Monitoring the file descriptors
        if (select(max_sd + 1, &read_fd, NULL, NULL, NULL) < 0){
            perror("Error: With the Select with multiple clients\n"); 
             exit(EXIT_FAILURE); 
		}

		if (FD_ISSET(server_socket, &read_fd)) {
			
			int add_len = sizeof(server_address);
			client_socket = accept(server_socket, (struct sockaddr *)&server_address, (socklen_t*)&add_len);
			int login_status = 0;

			read(client_socket, client_response, sizeof(client_response));
			message_from_client = string_to_message(client_response);
			
			if (message_from_client->type == LOGIN) {
				
				// decode the message and get username and password and store them in two seperate chars
				char buffer[3000]; 
				strcpy(buffer, message_from_client->data);
				printf("buffer %s\n", buffer);
				sscanf(buffer,"%[^,],%s",cID,cPW) ; // after decoding the message, store username here

				login_status = user_login(&account_list,cID,cPW, client_socket);
                printf("client socket: %d\n",client_socket);
                print_user_list(account_list);
				struct message * login_ACK = (struct message *)malloc(sizeof(struct message)); 

				printf("%s\n",cID);
				printf("%s\n", cPW);
				printf("login status %d\n",login_status);
				if (login_status == 1) { 
					login_ACK->type = LO_ACK; 
					login_ACK->size = 0; 
					strcpy(login_ACK->source, "server");
					printf("test 1");
					strcpy(login_ACK->data, "Login-successful"); 
					printf("test 2"); 
 			
                                        //Adding the client socket to the list of client sockets
                    for (int i = 0; i < MAXCLIENTS; ++i) {
                        if (client_list[i] == 0){
                            client_list[i] = client_socket;
                            break;
                        }
                    }
                                        
				} else if (login_status == -1) {  
					strcpy(error_message, "Password is wrong!\n"); 
					// create message for NACK 
					login_ACK->type = LO_NAK;
					login_ACK->size = strlen(error_message);
					strcpy(login_ACK->source, "server");
					strcpy(login_ACK->data, error_message);
					print_message(login_ACK);
				} else if (login_status == -2) {  
					strcpy(error_message, "User is not found!\n"); 
					// create message for NACK 
					login_ACK->type = LO_NAK;
					login_ACK->size = strlen(error_message);
					strcpy(login_ACK->source, "server");
					strcpy(login_ACK->data, error_message);
				} else if (login_status == -3) {  
					strcpy(error_message, "Already Logged In!\n");  
					login_ACK->type = LO_NAK;
					login_ACK->size = strlen(error_message);
					strcpy(login_ACK->source, "server");
					strcpy(login_ACK->data, error_message);
				} else if (login_status == 0) {  
					strcpy(error_message, "Unknown Error!\n"); 
					login_ACK->type = LO_NAK;
					login_ACK->size = strlen(error_message);
					strcpy(login_ACK->source, "server");
					strcpy(login_ACK->data, error_message);
				}

				printf("test 3");
				print_message(login_ACK);
				printf("test 4");
				decode_message(login_ACK, server_message);
				//send the message 

				send(client_socket, server_message, sizeof(server_message), 0); 
				free(login_ACK);
			} 
			else if (message_from_client->type == REGISTER) {
				// TODO: basically do the same above and add the new person to the list
                char buffer[3000];
                strcpy(buffer, message_from_client->data);
                printf("buffer %s\n", buffer);
                sscanf(buffer,"%[^,],%s",cID,cPW); // after decoding the message, store username here
                struct message * register_ACK = (struct message *)malloc(sizeof(struct message));
                int status = register_helper(&account_list, cID, cPW);
                
                if (status == 0) {
                    printf("unknown error\n");
                    strcpy(error_message, "Unknown Error!\n");
                    register_ACK->type = REGISTER_NAK;
                    register_ACK->size = strlen(error_message);
                    strcpy(register_ACK->source, "server");
                    strcpy(register_ACK->data, error_message);
                } else if (status == 1) {
                    printf("register successful\n");
                    strcpy(error_message, "register successful!\n");
                    register_ACK->type = REGISTER_ACK;
                    register_ACK->size = strlen(error_message);
                    strcpy(register_ACK->source, "server");
                    strcpy(register_ACK->data, error_message);
                } else {
                    printf("username exists: register failed\n");
                    //printf("username exists: register failed\n");
                    strcpy(error_message, "username exists: register failed\n");
                    register_ACK->type = REGISTER_NAK;
                    register_ACK->size = strlen(error_message);
                    strcpy(register_ACK->source, "server");
                    strcpy(register_ACK->data, error_message);
                }
                decode_message(register_ACK, server_message);
                //send the message
                update_list(account_list);

                send(client_socket, server_message, sizeof(server_message), 0);
                free(register_ACK);
			}
		}

		// going through all the clients 
		for (int i = 0; i < MAXCLIENTS; i++) { 
            printf("test2");
			 client_socket = client_list[i]; 
			
			if (FD_ISSET(client_socket, &read_fd)) { 
				 read(client_socket, client_response, sizeof(client_response));
				 message_from_client = string_to_message(client_response);
				 print_message(message_from_client);

				if (message_from_client->type == NEW_SESS) {   
                    strcpy(session_name, message_from_client->data); 
                    strcpy(cID, message_from_client->source);

                    struct message * newSession_ACK = (struct message *)malloc(sizeof(struct message)); 

                    session_list[session_to_create] = (struct session_struct*)malloc(sizeof(struct session_struct));
                    if (create_session(&(session_list[session_to_create]), session_name, session_to_create)) {

                        int index = session_to_join(session_name);
                        struct account_struct *temp = returnUser(account_list,cID);

                        bool status = add_user_to_session(&(session_list[session_to_create]), temp, session_to_create);


                        printf("-----\n");
                        printf("cid: %s\n", cID);
                        printf("status: %d\n", status);


                        newSession_ACK->type = NS_ACK; 
                        newSession_ACK->size = 0;

                        user_connect(&account_list, cID, session_to_create);

                        strcpy(newSession_ACK->source, "server");
                        strcpy(newSession_ACK->data, session_list[session_to_create]->session_name);
                        session_list[session_to_create]->isActive = 1;
                        session_to_create++; 
                    }

                    print_message(newSession_ACK);
                    decode_message(newSession_ACK, server_message);


                    send(client_socket, server_message, sizeof(server_message), 0);


				}
				else if (message_from_client->type == JOIN) {
                                    strcpy(session_name, message_from_client->data);
                                    strcpy(cID, message_from_client->source);
                                    struct message * join_ACK = (struct message *)malloc(sizeof(struct message));
                                    printf("cid: %s\n", cID);
                                    printf("server %s\n", session_name);
                                    int index = session_to_join(session_name);
                     
                                    strcpy(join_ACK->data, session_name);
 
 
                                    if (index == -1) {
                                        join_ACK->type = JN_NACK;
                                        strcat(join_ACK->data, ",");
                                        strcat(join_ACK->data, "Session not found!");
                                    } else {
                                        struct account_struct *temp = returnUser(account_list,cID);
                                        if (temp == NULL) {
                                            join_ACK->type = JN_NACK;
                                            strcat(join_ACK->data, ",");
                                            strcat(join_ACK->data, "Account not found!");
                                        } else {
                                            bool status = add_user_to_session(&(session_list[index]), temp, index);
                                                if (status == false) {
                                                    join_ACK->type = JN_NACK;
                                                    strcat(join_ACK->data, ",");
                                                    strcat(join_ACK->data, "Session inactive!");
                                                } else {
                                                    join_ACK->type = JN_ACK;
                                                    user_connect(&account_list, cID, index);
                                                    print_user_list(account_list); 
                                                }
                                        }
                                    }

                                    join_ACK->size = 0;
                                    strcpy(join_ACK->source, "server");
                    
                                    decode_message(join_ACK, server_message);
                    
                                    printf("Server:MESSAGE: %s\n", server_message);
                                    //send the message
                                    send(client_socket, server_message, sizeof(server_message), 0);
                                    free(join_ACK);
                    
				}

				else if (message_from_client->type == QUERY) {
						// return QU_ACK type message back to client with the message_to_client->data field
                        // being filled with the online users and sessions  

                                    char lists[2048] = "\nAll Logged in users:\n";
                                    struct account_struct *root = account_list;

                                    while (root != NULL) {
                                        if (root->loggedIn == 1) {
                                        strcat(lists,root->cID);
                                        strcat(lists,"\n");
                                        }
                                        root = root->next_user;
                                    }

                                    strcat(lists,"Active sessions:\n");
                                    for (int i = 0; i < max_sessions; i++) {
                                        if (session_list[i] != NULL) {
                                            if (session_list[i]->isActive) {
                                                char users_in_sess[] = "  - Users connected to this session: ";
                                                store_user_list(session_list[i]->connected_users, (users_in_sess));
                                                strcat(lists,session_list[i]->session_name);
                                                strcat(lists,"\n");
                                                strcat(lists,users_in_sess);
                                                strcat(lists,"\n");

                                                //print_users_in_session(session_list[i]);
                                            }
                                        } 
                                    } 

                                    struct message * query_ACK = (struct message *)malloc(sizeof(struct message));   
                                    query_ACK->type = QU_ACK; 
                                    query_ACK->size = strlen(lists);
                                    strcpy(query_ACK->source, "server");
                                    strcpy(query_ACK->data, lists);

                                    decode_message(query_ACK, server_message);
                                    memset(lists, 0, sizeof(lists));
                                    //send the message 
                                    if ((send(client_socket, server_message, sizeof(server_message), 0)) < 0) printf(" list ACK failed\n"); 
                                    free (query_ACK);
				}
				
				else if (message_from_client->type == EXIT) {  

                                    printf("test 1\n");
                                                        // make this client from online status to offline status  
                                                        // do not have to send acknowledgement back 
                                    // used for logout
                                    char buffer[3000]; 
                                                    strcpy(buffer, message_from_client->data);
                                    strcpy(cID, message_from_client->source);
                                                    printf("buffer %s\n", buffer);

                                    int session_to_leave = get_user_session(account_list, cID);

                                    if (session_to_leave >= 0) {
                                       // user_disconnect(&account_list, cID);
                                        remove_from_session_by_user(&(session_list[session_to_leave]), cID);
                                    }

                                    bool log = user_logout(&account_list, cID);
                                    printf("clientid: %s\n",cID);
                                    if (log) printf("logged out successfully\n");
                                    else printf("did not log out\n!");

                                    struct message * logout_ACK = (struct message *)malloc(sizeof(struct message));   
                                    logout_ACK->type = EXIT_ACK;
                                    strcpy(logout_ACK->source, "server");
                                    strcpy(logout_ACK->data, "Exited Server Successfuly");
                                    logout_ACK->size = strlen("Exited Server Successfuly");

                                    decode_message(logout_ACK, server_message);
                                    if ((send(client_socket, server_message, sizeof(server_message), 0)) < 0) printf(" logout ACK failed\n");
                                    close(client_socket);
                                    client_list[i] = 0; 
				} 

                else if (message_from_client->type == LEAVE_SESS) {
                    char buffer[3000]; 
				    strcpy(buffer, message_from_client->data);
                    strcpy(cID, message_from_client->source);
				    printf("buffer %s\n", buffer);

                    int session_to_leave = get_user_session(account_list, cID);

                    user_disconnect(&account_list, cID);
                    remove_from_session_by_user(&(session_list[session_to_leave]), cID);
                    
                    struct message * leavesess_ACK = (struct message *)malloc(sizeof(struct message));   
                    leavesess_ACK->type = LEAVE_ACK;
                    strcpy(leavesess_ACK->source, "server");
                    strcpy(leavesess_ACK->data,"hi");
                    leavesess_ACK->size = strlen("hi");
                    
                    decode_message(leavesess_ACK, server_message);

                    if ((send(client_socket, server_message, sizeof(server_message), 0)) < 0) printf(" list ACK failed\n");


                }
                else if (message_from_client->type == MESSAGE) {
					
                    // TODO: write code that checks the active session and 
					// send the message coming from a client to all the clients 
					// within the session 
                                  
                    char temp_buffer[2000]; 
                    memset(temp_buffer, 0, 2000);

                    char final_send[2000];
                    memset(final_send, 0, 2000);
                   
                    
                    strcpy(temp_buffer, "Message from ");
                    strcat(temp_buffer, message_from_client->source);
                    strcat(temp_buffer, ": ");
                    strcat(temp_buffer,  message_from_client->data);
                                    
                    struct message * send_message_to_clients = (struct message *)malloc(sizeof(struct message));
                    send_message_to_clients->type = MESSAGE;
                    send_message_to_clients->size = strlen(temp_buffer);
                    strcpy(send_message_to_clients->source, "server"); 
                    strcpy(send_message_to_clients->data, temp_buffer);

                    decode_message(send_message_to_clients, final_send);
                        
                    int session_to_send = get_user_session(account_list, message_from_client->source);
                    
                    if (session_list[session_to_send]->isActive) {
                        struct account_struct * temp = session_list[session_to_send]->connected_users;
                        while(temp != NULL) {
                            printf("source: %s\n", message_from_client->source);
                            printf("temp->cid %s\n", temp->cID);
                            if (strcmp(message_from_client->source,temp->cID)!=0) {
                                int send_client_socket = returnUser(account_list, temp->cID)->socket;
                                printf("send client socket: %d\n", send_client_socket);
                                send(send_client_socket, final_send, strlen(final_send), 0);
                                printf("sent successfully\n");
                            }
                            temp = temp->next_user;

                        }
                    }

                    send_message_to_clients->type = MESSAGE_ACK;

                    decode_message(send_message_to_clients, final_send);

                    send(client_socket, final_send, strlen(final_send), 0); 

				} 
			}
            printf("test 4\n");
		} 
        printf("test 3\n");
    }  
	
    printf("Text Conferencing APP successfully closed!");
    update_list(account_list);
	close(client_socket);
	return 0;  
}


