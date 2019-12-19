#include "globals.h"
#include "session.h"
#include "account.h"
#include "packet.h"

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
  //update_list(account_list); 
  new_account_char(&account_list, "jay", "test");
  new_account_char(&account_list, "anikeith", "test");
  printf("message\n");
  printf("%s\n",account_list->cPW);


  int session_to_create = 0;
	if (argc != 2) { // check if there's 2 arguments given, if not ...
        printf("Error 1:\n Invalid Usage\n Usage Format: server (TCP listen port), i.e., server 2000\n");
    }

	char server_message [2048]; 
	char client_response [2000]; 
	//create the server socket 
	int server_socket;  
	int port = atoi(argv[1]);
	int opt = 1;
	server_socket = socket(AF_INET, SOCK_STREAM, 0); // TCP socket using SOCK_STREAM and 0
	setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	if (server_socket < 0) {
       	printf("Error 2:\n TCP Socket Connection Error! Unable to establish connection with socket\n");
        return 0;
  	}

	//define the server address 
	struct sockaddr_in server_address; 
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(port);
	server_address.sin_addr.s_addr = htonl(INADDR_ANY); 

	//bind the socket to our specified IP and port 
	if (bind(server_socket, (struct sockaddr*) &server_address, sizeof(server_address))<0) {
		printf("bind failed\n");
	}
	listen(server_socket, 7);  


	int client_socket; 
	int add_len = sizeof(server_address);
	client_socket = accept(server_socket, (struct sockaddr *)&server_address, (socklen_t*)&add_len);
  	int login_status = 0;

  	// variables to be used in loop
  	char session_name[30];
  	char cID[cID_len];  
	char cPW[cPW_len]; 
	struct message * message_from_client;



	// look at what the client has sent (aka look at the message type field of the struct
	// using the string to packet function from packet.h)

	// have four if statements, one for login, one for createsession, one for joinsession, 
	// one for QUERY

	char error_message [100] ; 
	
	
	while(true) {
		
		read(client_socket, client_response, sizeof(client_response));
		message_from_client = string_to_message(client_response);
		//printf("client message\n%s\n", client_response);
		//print_message(message_from_client);
    	//receive the message here and decode it
		if (message_from_client->type == LOGIN) {
			
			// decode the message and get username and password and store them in two seperate chars
			char buffer[3000]; 
			strcpy(buffer, message_from_client->data);
			printf("buffer %s\n", buffer);
			sscanf(buffer,"%[^,],%s",cID,cPW) ; // after decoding the message, store username here
			//strcpy(cPW,message_from_client->data); // after decoding the message, store password here

			login_status = user_login(&account_list,cID,cPW);
			struct message * login_ACK = (struct message *)malloc(sizeof(struct message)); 

			printf("%s\n",cID);
			printf("%s\n", cPW);
			printf("login status %d\n",login_status);
			if (login_status == 1) { 
				login_ACK->type = LO_ACK; 
				login_ACK->size = 0; 
				strcpy(login_ACK->source, "server");
				strcpy(login_ACK->data, "\0"); 
				// create message for ACK
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
				// create message for NACK 
				login_ACK->type = LO_NAK;
				login_ACK->size = strlen(error_message);
				strcpy(login_ACK->source, "server");
				strcpy(login_ACK->data, error_message);
        	} else if (login_status == 0) {  
				strcpy(error_message, "Unknown Error!\n"); 
				// create message for NACK 
				login_ACK->type = LO_NAK;
				login_ACK->size = strlen(error_message);
				strcpy(login_ACK->source, "server");
				strcpy(login_ACK->data, error_message);
        	}

			print_message(login_ACK);
			decode_message(login_ACK, server_message);
			//send the message 


			send(client_socket, server_message, sizeof(server_message), 0); 
			free(login_ACK);
		}
		else if (message_from_client->type == NEW_SESS) {  
			strcpy(session_name, message_from_client->data); 
			strcpy(cID, message_from_client->source);

			struct message * newSession_ACK = (struct message *)malloc(sizeof(struct message)); 

			session_list[session_to_create] = (struct session_struct*)malloc(sizeof(struct session_struct));
			if (create_session(&(session_list[session_to_create]), session_name, session_to_create)) {

				int index = session_to_join(session_name);
				struct account_struct *temp = returnUser(account_list,cID);
				bool status = add_user_to_session(&(session_list[index]), temp, index);
				newSession_ACK->type = NS_ACK; 
				newSession_ACK->size = 0; 
				strcpy(newSession_ACK->source, "server");
				strcpy(newSession_ACK->data, session_list[session_to_create]->session_name);
				session_list[session_to_create]->isActive = 1;
				session_to_create++; 
			}
			//	return NS_ACL type message to client
			//} else {
			// doing nothing, apparently there is no negative acknowledge for new session specfically
			// or we can make our own negative acknowledgement for not creating a new session
			//}   

			print_message(newSession_ACK);
			decode_message(newSession_ACK, server_message);


			send(client_socket, server_message, sizeof(server_message), 0);

			// get cID from message and store it into cID varialbe using strcpy

		}
		else if (message_from_client->type == JOIN) { // it should never enter this if statement for this lab 
					// if (joining new session successful) {
					// 		return JN_ACK type message to client with message_to_client->data being the session_id
					//} else {
					//		return JN_NAK type message to client with message_to_client->data being the session_id
					//		and the reason for failure  
					//	}  
					int index = session_to_join(session_name);
					struct account_struct *temp = returnUser(account_list,cID);
					bool status = add_user_to_session(&(session_list[index]), temp, index);
					strcpy(session_name, message_from_client->data); 
					strcpy(cID, message_from_client->source);

					//send the message
					send(client_socket, server_message, sizeof(server_message), 0);
		}

		else if (message_from_client->type == QUERY) {
				// return QU_ACK type message back to client with the message_to_client->data field
				// being filled with the online users and sessions  
		
			char lists[2048] = "Logged in users:\n";
			struct account_struct *root = account_list;
			
			while (root != NULL) {
				if (root->loggedIn == 1) {
				strcat(lists,root->cID);
				strcat(lists,"\n");
				}
				root = root->next_user;
			}
			
			strcat(lists,"active sessions:\n"); 
			for (int i = 0; i < max_sessions; i++) {
				if (session_list[i] != NULL) {
					if (session_list[i]->isActive) {
						strcat(lists,session_list[i]->session_name);
						strcat(lists,"\n");
					}
				} 
			} 

			struct message * query_ACK = (struct message *)malloc(sizeof(struct message));   
			query_ACK->type = QU_ACK; 
			query_ACK->size = strlen(lists);
			strcpy(query_ACK->source, "server");
			strcpy(query_ACK->data, lists);
			
			decode_message(query_ACK, server_message);
			//send the message 
			if ((send(client_socket, server_message, sizeof(server_message), 0))<0) printf("failed\n");
		}
		
		else if (message_from_client->type == EXIT) {
			// send message back
			break;
		}
	} 
	close(client_socket);
	return 0; 
}


