#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>     //includes sockaddr
#include <netdb.h>          //includes addrinfo
#include <arpa/inet.h>      //includes inet_ntop etc
#include <unistd.h>         //includes close()
#include <stdlib.h>         //includes exit(1)
#include <errno.h>
#include <time.h>
#include <math.h>
#include <stdbool.h>

#define MAX_FRAG_SIZE 1000

struct packet {
    // creating a linked list to allow for packets to be connected
    struct packet * next_frag; // pointer to next fragment in sequence
    unsigned int total_frag; // total number of fragments for the file being sent
    unsigned int frag_no; // current packet fragment number
    unsigned int size;  // size (bytes) of current fragment (0 - 1000)
    char* filename; // file name
    char filedata[1000];  // file data being transmitted through this fragment
};

struct packet * segmentize_file(char * fname);
struct packet * unpack(char * packet_string);
char * compress_packet(struct packet * packet, int *pack_len);
int compressed_length(struct packet * pack);
void print_packet(struct packet * packets);
void free_packets(struct packet * curr_packet);
int compressed_length(struct packet * pack);

// we need a function to fragment the file into multiple fragments
struct packet * segmentize_file(char * fname) {
    FILE * filepointer;
    
    // opens a file in read only mode
    filepointer = fopen(fname, "rb"); // open the file in read only mode
    if (filepointer == NULL) { // error checking to see if the file is found or not
        printf("Failed to obtain file\n");
        return NULL;
    }
    
    int total_no_frags, byte_size;
    char frag_data[MAX_FRAG_SIZE];
    

    // getting file size -- seek to end of file, store the size in the variable, seek back
    fseek(filepointer, 0L, SEEK_END);
    byte_size = ftell(filepointer); // finds the size (0 to pointer)
    fseek(filepointer, 0L, SEEK_SET);   // seek baxk to start of the file

    // determine the number of fragments needed assuming each fragment has 1000 bytes of data
    // +1 to allow for storage space for any partial data
    total_no_frags = (byte_size/MAX_FRAG_SIZE) + 1;

    struct packet * head_pack = malloc(sizeof(struct packet));
    struct packet * last_linked_pack;

    
    // creating the head packet of linked list of packets
    int size = fread(frag_data, 1, MAX_FRAG_SIZE, filepointer); // reads the file and gets the size into the variable
                                                                // the pointer stops after 1000 bytes
    head_pack->next_frag = NULL;
    head_pack->total_frag = total_no_frags;
    head_pack->frag_no = 1;
    head_pack->size = size;
    head_pack->filename = fname;
    memcpy(head_pack->filedata, frag_data, size);
    last_linked_pack = head_pack;


    struct packet * new_pack;
    for (int frag_iterator = 2; frag_iterator <= total_no_frags; frag_iterator++) {
        new_pack = malloc(sizeof(struct packet));
        last_linked_pack->next_frag = new_pack;

        size = fread(frag_data, 1, MAX_FRAG_SIZE, filepointer); // reads the first 1000 bits of the file
        new_pack->next_frag = NULL;
        new_pack->total_frag = total_no_frags;
        new_pack->frag_no = frag_iterator;
        new_pack->size = size;
        new_pack->filename = fname;
        memcpy(new_pack->filedata, frag_data, size);
        last_linked_pack = new_pack;

        if (frag_iterator == total_no_frags) new_pack->next_frag = NULL;
    }

    fclose(filepointer); // stdio.h to close file
    return head_pack;   // return a pointer to the head packet
}

// we need a function to convert one fragment into a packet and run a for loop for this function
// to convert the entire file to packets
struct packet * unpack(char * packet_string) {

    struct packet * final_extracted_packet; // the final packet returned after being converted from string to packet
    unsigned int total_fragment, frag_no, size;
    
    char * total_frag_string = strtok(packet_string, ":");
    
    // array of strings that contains the packet header
    // index 0: total number of fragments
    // index 1: current fragment number
    // index 2: data size
    // index 3: file name/directory
    // packet data stored seperately as it is not being considered as a string
    // also need to obtain length of the header
    char * packet_data[4];
    char * filedata;
    unsigned int total_header_size = 0;
    
    for (int i = 0; i < 4; i++) {
        if (i == 0) packet_data[i] = total_frag_string;
        else packet_data[i] = strtok(NULL, ":");
        total_header_size += strlen(packet_data[i]);
    }
    
    total_header_size += 4; // pad header
    
    
    total_fragment = atoi(packet_data[0]); // atoi parses the string for an integral number and returns it as an integer
    frag_no = atoi (packet_data[1]);
    size = atoi(packet_data[2]);

    // reserving the safe amount of space for the file data
    filedata = malloc(size*sizeof(char));


    // safely using memcpy
    memcpy(filedata, &packet_string[total_header_size], size);
        
    // now assemble the final packet
    final_extracted_packet = malloc(sizeof(struct packet));
    memcpy(final_extracted_packet->filedata, filedata, size);
    final_extracted_packet->filename = packet_data[3];
    final_extracted_packet->frag_no = frag_no;
    final_extracted_packet->size = size;
    final_extracted_packet->total_frag = total_fragment;

    if (size < MAX_FRAG_SIZE) {
        final_extracted_packet->filedata[size] = '\0';
    }

    return final_extracted_packet;

}

// we need a function to convert one packet back to a string
char * compress_packet(struct packet * pack, int * pack_len) {
    char *temp;

	unsigned int total_size = snprintf(NULL,0,"%d", pack->total_frag) + snprintf(NULL,0,"%d", pack->frag_no)
							  + snprintf(NULL,0,"%d", pack->size) + strlen(pack->filename) + pack->size;

    char * total_compressed_packet = malloc((total_size+4)*sizeof(char)); // safely reserve memory for the content to be sent

    // we need to save enough buffer room for the header content
    unsigned int size_header_of_packet = sprintf(total_compressed_packet, "%d:%d:%d:%s:", pack->total_frag, pack->frag_no,pack->size,pack->filename);

    // using memcpy because data may be in binary
    memcpy(&total_compressed_packet[size_header_of_packet], pack->filedata, pack->size);
    
	*pack_len = size_header_of_packet + pack->size;
    return total_compressed_packet; // finally return the actual character string
}

int compressed_length(struct packet * pack) {
    char *total_compressed_packet = malloc(sizeof(char));
    unsigned int size_header_of_packet = sprintf(total_compressed_packet, "%d:%d:%d:%s:",pack->total_frag, pack->frag_no,pack->size,pack->filename);
    return (size_header_of_packet + pack->size);
}

// we need a function to print a packet
void print_packet(struct packet * packets) {
    printf("total_frag = %d,\nfrag_no = %d,\nsize = %d,\nfilename = %s,\nfiledata = %s\n",
           packets->total_frag,
           packets->frag_no,
           packets->size,
           packets->filename,
           packets->filedata);
}




