#include <time.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define DEST_IP     argv[1]
#define DEST_PORT   argv[2]
#define MSG_FILE    argv[3]
#define PROTOCOL    argv[4]

#define TIMEOUT 1

#define SEND_PROB   95

#define SYN_PKT     0
#define SYNACK_PKT  1
#define ACK_PKT     2
#define RST_PKT     3
#define DATA_PKT    4
#define FIN_PKT     5
#define FINACK_PKT  6

#define PKT_DATA_SIZE   2

time_t t;

int sockfd;
int rv;
int numbytes;
int resend_count;

int is_fastmode;

struct packet out_pkt;
struct packet in_pkt;

struct addrinfo hints, *servinfo, *p;
struct sockaddr_storage their_addr;
struct packet out_pkt;
struct packet out_pkt_1;
struct packet out_pkt_2;
struct packet in_pkt;

socklen_t addr_len;

char s[INET6_ADDRSTRLEN];
char * buf;

struct packet {
    uint8_t type;
    uint8_t seqnum;
    uint16_t checksum;
    char *data;
};

uint16_t checksum (uint8_t type, uint8_t seqnum, char *data) {

    // Add packet type, sequence number, and packet data together
    // then take the inverse of all bits

    int sum = type + seqnum; // Add packet type and sequence number

    // If packet is a DATA packet, add data into checksum
    if(type == DATA_PKT) {

        // Loop through the data string
        while (*data != 0) {

            // Check for overflow (greater than 2^16)
            if ((sum + *data) > 65535)
                sum -= 65535;

            // Add each character into checksum
            sum += *data;
            data++; // increment data pointer to point to the next char
        }

    }

    return ~sum; // Inverse all bits

}

void serialize_pkt(char *buf, struct packet pkt) {

    // Calculate checksum
    pkt.checksum = checksum(pkt.type, pkt.seqnum, pkt.data);

    // Put packet header and data into string
    if (pkt.type != DATA_PKT)
        sprintf(buf, "%d-%d-%d-", pkt.type, pkt.seqnum, pkt.checksum);
    else
        sprintf(buf, "%d-%d-%d-%s-", pkt.type, pkt.seqnum, pkt.checksum, pkt.data);

}

void deserialize_pkt(char *buf, struct packet *pkt) {

    // De-structurize received data 'type-seqnum-checksum-data-'
    // Cut string by finding '-'
    pkt->type = atoi(strtok(buf, "-"));
    //printf("Type: %d\n", pkt->type);
    pkt->seqnum = atoi(strtok(NULL, "-"));
    //printf("Seqnum: %d\n", pkt->seqnum);
    pkt->checksum = atoi(strtok(NULL, "-"));
    //printf("Checksum: %d\n", pkt->checksum);

    // If packet is a DATA packet get data from string
    if (pkt->type == DATA_PKT) {
        pkt->data = strtok(NULL, "-");
        //printf("Data: %s\n", pkt->data);
    } else 
        pkt->data = NULL;

}

ssize_t perhaps_sendto() {

    int pb;

    // Get a random number [0-100] that will dictate if sendto will "succeed"
    pb = rand() % 101;

    if (pb <= SEND_PROB)
        return sendto(sockfd, buf, strlen(buf), 0, p->ai_addr, p->ai_addrlen);
    else
        puts("Failed to send");
    
    return 0;

}

void timeout(int sig) {

    // Update resend counter
    resend_count++;
    puts("No ACK");

    // Kill connection if resend counter is greater than or equal to 5
    if (resend_count >= 5) {
        fprintf(stderr, "Connection lost.\n");
        close(sockfd);
        exit(1);
    }

    // Put 2nd oldest packet struct data into a single string
    serialize_pkt(buf, out_pkt_1);

    // Try to resend 2nd oldest packet
    if ((numbytes = perhaps_sendto()) == -1) {
        perror("sender: sendto");
        exit(1);
    }

    switch (out_pkt_1.type) {

        case SYN_PKT:
            puts("Sent SYN");
            break;
        case FIN_PKT:
            puts("Sent FIN");
            break;
        case DATA_PKT:
            puts("Sent DATA");
            break;

    }

    printf("sender: sent \"%s\"\n", buf);

    // If window size is two
    if (is_fastmode) {

        // Put oldest packet struct data into a single string
        serialize_pkt(buf, out_pkt_2);

        // Try to resend oldest packet
        if ((numbytes = perhaps_sendto()) == -1) {
            perror("sender: sendto");
            exit(1);
        }


        switch (out_pkt_2.type) {

            case SYN_PKT:
                puts("Sent SYN");
                break;
            case FIN_PKT:
                puts("Sent FIN");
                break;
            case DATA_PKT:
                puts("Sent DATA");
                break;

        }

        printf("sender: sent \"%s\"\n", buf);

    }

    // Turn off fast mode
    if (is_fastmode)
        puts("---------- SLOW MODE ----------");
    is_fastmode = 0;

    // Re-define function that will handle the SIGALRM signal from alarm
    signal(sig, timeout);

    // Restart timer
    alarm(TIMEOUT);

}

int est_connection(int sockfd, struct sockaddr *ai_addr, socklen_t ai_addrlen, struct sockaddr_storage *their_addr, socklen_t *addr_len) {

    buf = malloc(20);

    // Set packet to be of type SYN
    out_pkt.type = SYN_PKT;
    out_pkt.seqnum = 1;

    // Put packet struct data into a single string
    serialize_pkt(buf, out_pkt);

    // Try to send SYN
    if ((numbytes = perhaps_sendto()) == -1) {
        perror("sender: sendto");
        exit(1);
    }

    // Trigger SIGALRM signal after a set amount of time and clear resend count
    resend_count = 0;
    alarm(TIMEOUT);

    printf("sender: sent \"%s\"\n", buf);
    printf("Sent SYN\n");

    // Wait for SYNACK
    *addr_len = sizeof(their_addr);
    while ((numbytes = recvfrom(sockfd, buf, sizeof(buf) - 1, 0, (struct sockaddr *) their_addr, addr_len)) == -1);

    // Stop timer and clear resend count
    alarm(0);
    resend_count = 0;

    printf("Got SYNACK\n\n");

    return 1;

}

int end_connection(int sockfd, struct sockaddr *ai_addr, socklen_t ai_addrlen, struct sockaddr_storage *their_addr, socklen_t *addr_len) {

    buf = malloc(20);

    // Set packet to be of type FIN
    out_pkt.type = FIN_PKT;

    // Put packet struct data into a single string
    serialize_pkt(buf, out_pkt);

    // Try to send FIN
    if ((numbytes = perhaps_sendto()) == -1) {
        perror("sender: sendto");
        exit(1);
    }

    // Start timer and clear resend count
    resend_count = 0;
    alarm(TIMEOUT);

    printf("sender: sent \"%s\"\n", buf);
    printf("Sent FIN\n");

    // Wait for FINACK
    *addr_len = sizeof(their_addr);
    while ((numbytes = recvfrom(sockfd, buf, sizeof(buf) - 1, 0, (struct sockaddr *) their_addr, addr_len)) == -1);

    // Stop timer and clear resend count
    alarm(0);
    resend_count = 0;

    printf("Got FINACK\n");

    // Close socket
    close(sockfd);

    return 1;

}

int main (int argc, char *argv[]) {

    FILE *fp;

    // Allocate the appropriate amount of memory to pointers
    buf = malloc(20);
    out_pkt.data = malloc(PKT_DATA_SIZE);
    out_pkt_1.data = malloc(PKT_DATA_SIZE);
    out_pkt_2.data = malloc(PKT_DATA_SIZE);
    in_pkt.data = malloc(PKT_DATA_SIZE);

    // Set randomizer seed
    srand((unsigned) time(&t));

    // Establish handler for SIGALRM signal
    signal(SIGALRM, timeout);
        
    // Check if user has correct number of arguments.
    if (argc != 5) {
        fprintf(stderr, "Usage: sender dest_IP dest_port filname proto\n");
        exit(1);
    }

    // Open file to be read and check for errors
    if((fp = fopen(MSG_FILE, "rb")) == NULL) {
        fprintf(stderr, "sender: fopen");
        exit(1);
    } 

    // Clear hints struct
    memset(&hints, 0, sizeof(hints));

    hints.ai_family = AF_UNSPEC;    // Set family to be IP version agnostic
    hints.ai_socktype = SOCK_DGRAM; // Set socket to be a UDP connection

    // Get address information and check for errors
    if ((rv = getaddrinfo(DEST_IP, DEST_PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // Loop through the results and bind a socket to the first usable address
    for (p = servinfo; p != NULL; p = p->ai_next) {

        // Create a socket to be used for the file transfer
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            perror("sender: socket");
            continue;
        }

        break;

    }
    
    // Check to see if no usable address was found
    if (p == NULL) {
        fprintf(stderr, "sender: failed to create a socket\n");
        return 2;
    }

    // Set sockets no be non-blocking
    // This allows polling to be done, although quite CPU intensive
    fcntl(sockfd, F_SETFL, O_NONBLOCK);

    // Try to establish a connection
    if (est_connection(sockfd, p->ai_addr, p->ai_addrlen, &their_addr, &addr_len) == -1) {
        fprintf(stderr, "sender: failed to establish a connection");
        exit(1);
    }

    // Set succeeding packets to be of type DATA and start with random sequence number
    out_pkt.type = DATA_PKT;
    out_pkt.seqnum = rand() % 256 + 1;

    // Set the base sequence number of the varying window and the sequence number of the next packet to be sent
    uint8_t base = out_pkt.seqnum;
    uint8_t next_seqnum = base;

    while (1) {

        // Clear buf
        memset(buf, 0, 20);

        // If the sequence number of the next packet is within the sliding window
        if(next_seqnum < (base + is_fastmode + 1)){

            // Get data from file and end connection when done reading file
            if(fgets(out_pkt.data, PKT_DATA_SIZE, fp) == NULL) {
                
                // Start the FIN-FINACK sequence
                if (end_connection(sockfd, p->ai_addr, p->ai_addrlen, &their_addr, &addr_len) == -1) {
                    fprintf(stderr, "sender: failed to terminate connection");
                    exit(1);
                }

                break;

            }

            // Put packet struct data into a single string
            serialize_pkt(buf, out_pkt);

            // Send serialized string
            if ((numbytes = perhaps_sendto()) == -1) {
                perror("sender: sendto");
                exit(1);
            }

            // If the base of the window is equal to the sequence number of the next packet,
            // clear resend count and start timer
            if(base == next_seqnum){
                resend_count = 0;
                alarm(TIMEOUT);
            }

            // Copy the 2nd oldest packet into the oldest packet struct
            out_pkt_2.type = out_pkt_1.type;
            out_pkt_2.seqnum = out_pkt_1.seqnum;
            out_pkt_2.checksum = out_pkt_1.checksum;
            strcpy(out_pkt_2.data, out_pkt_1.data);

            // Copy the just sent packet into the 2nd oldest packet struct
            out_pkt_1.type = out_pkt.type;
            out_pkt_1.seqnum = out_pkt.seqnum;
            out_pkt_1.checksum = out_pkt.checksum;
            strcpy(out_pkt_1.data, out_pkt.data);

            // Update seqnum with number of bytes sent
            out_pkt.seqnum++;
            // Update next sequence number aswell 
            next_seqnum++;

            printf("sender: sent \"%s\" to %s\n", buf, DEST_IP);

            puts("Sent DATA");

        }

        // Clear buf
        memset(buf, 0, 20);

        // Set to the size of the address
        addr_len = sizeof(their_addr);

        // Poll for ACK, skip if nothing is waiting in the socket
        if ((numbytes = recvfrom(sockfd, buf, strlen(buf) - 1, 0, (struct sockaddr *) &their_addr, &addr_len)) != -1) {

            printf("sender: packet contains \"%s\"\n", buf);

            // Decompose string into type, seqnum, checksum, data
            deserialize_pkt(buf, &in_pkt);

            // Check for integrity of the packet
            if(in_pkt.checksum != checksum(in_pkt.type, in_pkt.seqnum, in_pkt.data))
                continue;
            
            // Make sure it is an ACK packet
            if (in_pkt.type == ACK_PKT)  {
                
                // Set base of window to be 1 more than the received packet's sequence number
                base = in_pkt.seqnum + 1;

                // When base of window is equal to the next packet's sequence number
                if(base == next_seqnum) {

                    // Stop timer and clear resend count
                    alarm(0);
                    resend_count = 0;

                    // Turn on fast mode
                    is_fastmode = 1;
                    puts("---------- FAST MODE -----------");

                } else
                    alarm(TIMEOUT); // Start timer

                printf("Base: %d\tNext: %d\n", base, next_seqnum);
                printf("Got ACK %d\n\n", in_pkt.seqnum);

            }
    
        }

    }

    // Free pointers
    free(buf);
    free(out_pkt.data);
    free(in_pkt.data);
    freeaddrinfo(servinfo);
    fclose(fp);

    return 0;

}