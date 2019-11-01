#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define SRC_PORT argv[1]
#define OUT_FILE argv[2]

#define TIMEOUT 6

#define SEND_PROB   95

#define SYN_PKT     0
#define SYNACK_PKT  1
#define ACK_PKT     2
#define RST_PKT     3
#define DATA_PKT    4
#define FIN_PKT     5
#define FINACK_PKT  6

#define PKT_DATA_SIZE 2

time_t t;

FILE *fp;

int sockfd;
int rv;
int numbytes;

struct addrinfo hints, *servinfo, *p;
struct sockaddr_storage their_addr;
struct packet out_pkt;
struct packet in_pkt;

socklen_t addr_len;

char s[INET6_ADDRSTRLEN];
char *buf;

struct packet {
    uint8_t type;
    uint8_t seqnum;
    uint16_t checksum;
    char *data;
};

ssize_t perhaps_sendto() {

    int pb;

    // Get a random number [0-100] that will dictate if sendto will "succeed"
    pb = rand() % 101;

    if (pb <= SEND_PROB)
        return sendto(sockfd, buf, strlen(buf), 0, (struct sockaddr *) &their_addr, addr_len);
    else 
        puts("Failed to send");

    return 0;

}

void timeout(int sig) {

    // After a few seconds without any input to the socket, kill the connection

    puts("Connection lost.");
    fclose(fp);
    exit(1);

}

uint16_t checksum (uint8_t type, uint8_t seqnum, char *data) {

    // Add packet type, sequence number, and packet data together
    // then take the inverse of all bits

    int sum = type + seqnum; // Add packet type and sequence number

    // If packet is a DATA packet, add data into the checksum
    if (type == DATA_PKT) {

        // Loop through the data string
        while (*data != 0) {

            // Check for overflow (greather than 2^16)
            if ((sum + *data) > 65535)
                sum -= 65535;

            // Add each character into the checksum
            sum += *data;
            data++; // Increment data pointer to point to the next character
        }

    }

    return ~sum;    // Invert all bits

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

    // If packet is a DATA packet, get data from string
    if (pkt->type == DATA_PKT) {
        pkt->data = strtok(NULL, "-");
        //printf("Data: %s\n", pkt->data);
    } else 
        pkt->data = NULL;

}

// Return sockaddr depending on whether address is IPv4 or IPv6
void *get_in_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET)
        return &(((struct sockaddr_in*)sa)->sin_addr);
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main (int argc, char *argv[]) {

    // Define a function to handle the signal that alarm will send (SIGALRM)
    signal(SIGALRM, timeout);

    // Allocate appropriately sized memory spaces to the string pointers
    buf = malloc(20);
    out_pkt.data = malloc(PKT_DATA_SIZE);
    in_pkt.data = malloc(PKT_DATA_SIZE);
        
    // Check if user has correct number of arguments.
    if (argc != 3) {
        fprintf(stderr, "Usage: receiver src_port filename\n");
        exit(1);
    }

    // Open file to be written to and check for errors
    if((fp = fopen(OUT_FILE, "w")) == NULL) {
        fprintf(stderr, "receiver: fopen");
        exit(1);
    } 

    // Clear hints struct
    memset(&hints, 0, sizeof(hints));

    hints.ai_family = AF_UNSPEC;    // Set family to be IP version agnostic
    hints.ai_socktype = SOCK_DGRAM; // Set socket to be a UDP connection
    hints.ai_flags = AI_PASSIVE;    // Set to use this machine's IP

    // Get address information and check for errors
    if ((rv = getaddrinfo(NULL, SRC_PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    } 

    // Loop through the results and bind a socket to the first usable address
    for (p = servinfo; p != NULL; p = p->ai_next) {

        // Create a socket to be used for the file transfer
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            perror("receiver: socket");
            continue;
        }
        
        // Bind socket to port
        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            perror("receiver: bind");
            close(sockfd);
            continue;
        }

        break;

    }

    // Check to see if no usable address was found
    if (p == NULL) {
        fprintf(stderr, "receiver: failed to bind a socket\n");
        return 2;
    }

    // Clear buf
    memset(buf, 0, 20);

    uint8_t expected_seqnum;

    while(1) {

        printf("receiver: waiting to recvfrom...\n");

        // Clear buf
        memset(buf, 0, strlen(buf));

        // Start the timer for timeout condition
        alarm(TIMEOUT);

        // Wait for packet
        addr_len = sizeof(their_addr);
        if ((numbytes = recvfrom(sockfd, buf, strlen(buf) - 1, 0, (struct sockaddr *) &their_addr, &addr_len)) == -1) {
            perror("receiver: recvfrom");
            exit(1);
        }

        // Stop timer since a packet was received
        alarm(0);

        printf("receiver: got packet from %s\n", inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *) &their_addr), s, sizeof(s)));
        printf("receiver: packet is %d bytes long\n", numbytes);
        printf("receiver: packet contains \"%s\"\n", buf);
        
        // Decompose packet string into type, seqnum, checksum, data
        deserialize_pkt(buf, &in_pkt);

        // For SYN_PKT
        if (in_pkt.type == SYN_PKT) {

            printf("Got SYN\n");

            // Set packet to be a SYNACK packet
            out_pkt.type = SYNACK_PKT;
            out_pkt.seqnum = 1;
            out_pkt.checksum = checksum(out_pkt.type, out_pkt.seqnum, out_pkt.data);

            // Put packet struct data into a single string
            serialize_pkt(buf, out_pkt);

            // Send SYNACK packet to sender
            if ((numbytes = perhaps_sendto()) == -1) {
                perror("sender: sendto");
                exit(1);
            }

            printf("receiver: sent %d bytes\n", numbytes);
            printf("%s\n", buf);
            printf("Sent SYNACK\n\n");

            continue;

        }

        // For FIN_PKT
        if (in_pkt.type == FIN_PKT) {

            printf("Got FIN\n");

            // Set packet as FINACK packet
            out_pkt.type = FINACK_PKT;
            out_pkt.seqnum = in_pkt.seqnum;
            out_pkt.checksum = checksum(out_pkt.type, out_pkt.seqnum, out_pkt.data);

            // Put packet struct data into a single string
            serialize_pkt(buf, out_pkt);

            // Send FINACK packet to sender
            if ((numbytes = perhaps_sendto()) == -1) {
                perror("sender: sendto");
                exit(1);
            }

            printf("receiver: sent %d bytes\n", numbytes);
            printf("%s\n", buf);
            printf("Sent FINACK\n\n");

            // Close file and socket
            fclose(fp);
            close(sockfd);

            break;

        }

        // For DATA_PKT
        // Check if checksums are equal
        if(in_pkt.type == DATA_PKT && (in_pkt.checksum == checksum(in_pkt.type, in_pkt.seqnum, in_pkt.data))) {

            printf("Got DATA\n");

            // If the received packet is the first packet in the DATA sequence
            // Set the expected sequence number to the first, random sequence number of packet
            if (out_pkt.type == SYNACK_PKT) {
                expected_seqnum = in_pkt.seqnum;
            }

            // If the received packet's sequence number is equal to the expected
            if (expected_seqnum == in_pkt.seqnum) {

                // Write data into file
                fputs(in_pkt.data, fp);

                // Update ACK sequence number to match the received
                out_pkt.seqnum = in_pkt.seqnum;
                expected_seqnum++; // Add 1 to the expected sequence number

            }

            // Set packet to be an ACK packet
            out_pkt.type = ACK_PKT;
            out_pkt.checksum = checksum(out_pkt.type, out_pkt.seqnum, out_pkt.data);

            // Put packet struct data into a single string
            serialize_pkt(buf, out_pkt);

            // Send ACK packet to sender
            if ((numbytes = perhaps_sendto()) == -1) {
                perror("sender: sendto");
                exit(1);
            }

            printf("receiver: sent \"%s\"\n", buf);
            printf("Sent ACK\n\n");

        }
        
    }

    // Free pointers
    free(buf);
    free(out_pkt.data);
    free(in_pkt.data);
    freeaddrinfo(servinfo);

    return 0;

}