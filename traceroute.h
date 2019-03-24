// Patryk Kumor
// indeks: 292601
#ifndef traceroute_h
#define traceroute_h

using namespace std;




#include <iostream>

#include <netinet/ip_icmp.h> //header
#include <netinet/ip.h> //sockety
#include <arpa/inet.h> //sockety
 
       #include <sys/types.h> //getpid
       #include <unistd.h>

#include <ctime>
#include <chrono>

#include <cassert> // do assert z funk. z wykładu
#include <stdint.h> // uinty

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

pid_t getpid(void);

typedef struct packet_info
{
    char *ip;
    int id;
    int ttl;
    time_t time;
    bool dest;
} packet_info;

u_int16_t compute_icmp_checksum (const void *buff, int length);
int single_send(int sockfd, int ttl, int process_id, struct sockaddr_in address);
void send(int sockfd, int ttl, int process_id, struct sockaddr_in address);


packet_info get_packet(int sockfd, int start_time);
int receive(int sockfd, int ttl, int process_id, clock_t start_time);






#endif
