// Patryk Kumor
// indeks: 292601
#include "traceroute.h"

u_int16_t compute_icmp_checksum (const void *buff, int length) // Wykład
{
	u_int32_t sum;
	const u_int16_t* ptr = (const u_int16_t*) buff; // żeby działało w cppp
	assert (length % 2 == 0);
	for (sum = 0; length > 0; length -= 2)
		sum += *ptr++;
	sum = (sum >> 16) + (sum & 0xffff);
	return (u_int16_t)(~(sum + (sum >> 16)));
}

int single_send(int sockfd, int process_id, int ttl, struct sockaddr_in address, int i)
{
    // Tworzenie nagłówka (wystarczy do echo)
    struct icmphdr icmp_header;
    icmp_header.type = ICMP_ECHO;
    icmp_header.code = 0;
    icmp_header.un.echo.id = process_id;
    icmp_header.un.echo.sequence = (ttl - 1)*3 + i; //unikalny dla wszystkch pakietów
    icmp_header.checksum = 0;
    icmp_header.checksum = compute_icmp_checksum ((u_int16_t*)&icmp_header, sizeof(icmp_header));
    // Wysyłanie
    setsockopt(sockfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl));
    ssize_t bytes_sent = sendto(sockfd, &icmp_header, sizeof(icmp_header), 0, (struct sockaddr*)&address, sizeof(address));
    if ( bytes_sent <= 0 )
    {
        throw runtime_error("Nie wysłano pakietu-single");
	}
    return 1;
}

void send(int sockfd, int ttl, int process_id, struct sockaddr_in address)
{
    for(int i = 0; i < 3; i++)
    {
        if (single_send(sockfd, process_id, ttl, address, i) < 0) throw runtime_error("Nie można wysłać pakietów");
    }
}
