#ifndef GLOBALS_H
#define GLOBALS_H


// various definitions
#define cID_len 30 // max username length
#define cPW_len 30 // max password length
#define max_sessions 3 // max number of sessions
#define MAX_DATA 2046 // max data


// include the following libraries
//include "packet.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>     //includes sockaddr
#include <netdb.h>          //includes addrinfo
#include <arpa/inet.h>      //includes inet_ntop etc
#include <unistd.h>         //includes close()
#include <errno.h>
#include <time.h>
#include <math.h>
#include <stdbool.h>
#include <ctype.h>
#include <math.h> 


typedef enum type {
    LOGIN = 1,  // login with the server <client ID, psswd)
    LO_ACK, 	// Ack successful login 
    LO_NAK, 	// login was unsuccessful 
    EXIT, 		// exit from the server 
    JOIN, 		// join the server 
    JN_ACK, 	// Ack for successful joining of server 
    JN_NACK, 	// joining server was unsuccessful 
    LEAVE_SESS, // leave conference session 
    NEW_SESS, 	// create a new conference session 
    NS_ACK,  	// Ack for new conference session 
    MESSAGE, 	// Send a message to the session or display the message 
    QUERY, 		// list of all online users and availabe sessions 
    QU_ACK		// reply followed by a list of users online 
} type;

// database of users
extern char dir[100];

// initialization of helper functions from helperfunctions.c
// this function parses the input and returns the type of command



#endif
