// Patryk Kumor
// indeks: 292601
#include "traceroute.h"

packet_info get_packet(int sockfd, int start_time)
{
            packet_info packet = {NULL, -1, -1, clock(), false};

		    struct sockaddr_in 	sender;	
		    socklen_t 			sender_len = sizeof(sender);
		    u_int8_t 			buffer[IP_MAXPACKET];

		    ssize_t packet_len = recvfrom (sockfd, buffer, IP_MAXPACKET, MSG_DONTWAIT, (struct sockaddr*)&sender, &sender_len);
            clock_t end_time = clock();
            //cout << "\nTime start :" << start_time << "\n";
            //cout << "\nTime end :" << end_time << "\n";
            
		    if (packet_len < 0) 
            {
			    //throw runtime_error("Recvfrom error\n"); // *zdjęcie czaszki*
                return packet;
		    }

		    char sender_ip_str[20]; 
		    inet_ntop(AF_INET, &(sender.sin_addr), sender_ip_str, sizeof(sender_ip_str));

            struct iphdr *ip_header = (struct iphdr *)buffer;
	        struct icmphdr *icmp_header = (struct icmphdr *)(buffer + 4 * ip_header->ihl);
            if (icmp_header->type == ICMP_ECHOREPLY) // NASZ CEL
	        {
                packet = {sender_ip_str, icmp_header->un.echo.id, 1 + (icmp_header->un.echo.sequence)/3, (end_time - start_time), true};
                /*
                cout << "\nTIME:" << (end_time - start_time);
                cout <<"\nIP:"<< sender_ip_str;
                cout <<"\nID:"<< icmp_header->un.echo.id;
                cout <<"\nSEQ:"<< icmp_header->un.echo.sequence;
                cout << "\n---------------------------";
                */
            }
	        if (icmp_header->type == ICMP_TIME_EXCEEDED)
        	{   
		        u_int8_t *icmp_packet = buffer + 4 * ip_header->ihl + 8; /* ip header length */
		        icmp_packet += 4 * ((struct ip *)(icmp_packet))->ip_hl;
		        struct icmp *icmp_time_ex = (struct icmp *)icmp_packet;
                packet = {sender_ip_str, icmp_time_ex->icmp_id, 1 + (icmp_time_ex->icmp_seq)/3, (end_time - start_time), false};
                /*
                cout << "\nTIME:" << (end_time - start_time);
                cout <<"\nIP:"<< sender_ip_str;
                cout <<"\nID:"<< icmp_time_ex->icmp_id;
                cout <<"\nSEQ:"<< icmp_time_ex->icmp_seq;
                cout << "\n---------------------------";
                */
            }
    return packet;
}

int receive(int sockfd, int ttl, int process_id, clock_t start_time)
{
    int how_many = 0; packet_info packets[3];

    fd_set descriptors;
    FD_ZERO (&descriptors);
    FD_SET (sockfd, &descriptors);
    struct timeval tv; tv.tv_sec = 1; tv.tv_usec = 0;
    
    while (how_many < 3)
    {
        int ready = select (sockfd+1, &descriptors, NULL, NULL, &tv); //NIE POZA PĘTLĄ. NIE.
        if (ready < 0) 
        {
            //throw runtime_error("Select error");// *zdjęcie czaszki*
            break;
        }
        if (ready == 0)
        { 
            // timeout
            break;
        }
        else
        {
            packet_info packet = get_packet(sockfd, start_time);
            if ((packet.id == process_id) && (packet.ttl == ttl))
            {
                packets[how_many]=packet;
                how_many++;
            }
        }
    }
    if (how_many == 0)
    {
        cout << ttl << ". *" <<endl; return 0;
    }
    else if (how_many == 3)
    {
        int time = (packets[0].time + packets[1].time + packets[2].time)/3;
        cout << ttl << ". " << packets[0].ip << " " << time << "ms" <<endl;
        if (packets[0].dest) {return 1;} else {return 0;} 
    }
    else
    {
        cout << ttl << ". " << packets[0].ip << " ??" <<endl;
        if (packets[0].dest) {return 1;} else {return 0;} 
    }
}
