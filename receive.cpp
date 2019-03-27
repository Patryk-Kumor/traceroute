// Patryk Kumor
// indeks: 292601
#include "traceroute.h"

packet_info get_packet(int sockfd,  struct timeval tvalBefore)
{
    struct timeval tvalAfter; gettimeofday (&tvalAfter, NULL);
    packet_info packet = {NULL, -1, -1, -1, false};
    // Odbieranie pakietu zgodnie z wykładem
    struct sockaddr_in 	sender;
    socklen_t sender_len = sizeof(sender);
    u_int8_t buffer[IP_MAXPACKET];
    if (recvfrom (sockfd, buffer, IP_MAXPACKET, MSG_DONTWAIT, (struct sockaddr*)&sender, &sender_len) < 0) 
    {
        //throw runtime_error("Recvfrom error\n");
        return packet; // Zwracamy pakiet "-1"
    }
    // Odbiór adresu IP
    char sender_ip_str[20]; 
    inet_ntop(AF_INET, &(sender.sin_addr), sender_ip_str, sizeof(sender_ip_str));
    // Przetwarzanie nagłówka pakietu zgodnie z wykładem
    struct iphdr *ip_header = (struct iphdr *)buffer;
    u_int8_t* icmp_packet = buffer + 4 * ip_header->ihl;
    struct icmphdr *icmp_header = (struct icmphdr*) icmp_packet;
    // ROUTERY PO DRODZE (mają informacje w stukturze bitowej o przekroczeniu czasu ttl -> należy przesunąć żeby odczytać)
    if (ICMP_TIME_EXCEEDED == icmp_header->type) 
    {
        // Przesunięcie bitowe
        u_int8_t *icmp_packet = buffer + 4 * ip_header->ihl + 8;
        icmp_packet += 4 * ((struct ip *)(icmp_packet))->ip_hl;
        struct icmp *icmp_te = (struct icmp *)icmp_packet;
        // Zapis informacji o pakiecie
        packet = {sender_ip_str,
                  icmp_te->icmp_id,
                  1 + (icmp_te->icmp_seq)/3,
                  (tvalAfter.tv_sec - tvalBefore.tv_sec)*1000000 + tvalAfter.tv_usec - tvalBefore.tv_usec,
                  false};
    }
    // NASZ CEL
    if (ICMP_ECHOREPLY == icmp_header->type)
    {
        // Zapis informacji o pakiecie
        packet = {sender_ip_str,
                  icmp_header->un.echo.id,
                  1 + (icmp_header->un.echo.sequence)/3,
                  (tvalAfter.tv_sec - tvalBefore.tv_sec)*1000000 + tvalAfter.tv_usec - tvalBefore.tv_usec,
                  true};
    }
    return packet;
}

int receive(int sockfd, int ttl, int process_id, struct timeval tvalBefore)
{
    int how_many = 0; packet_info packets[3];
    // Oczekiwanie na pakiety zgodnie z wykładem
    packet_info packet;
    fd_set descriptors;
    FD_ZERO (&descriptors);
    FD_SET (sockfd, &descriptors);
    struct timeval tv; tv.tv_sec = 1; tv.tv_usec = 0;
    int ready;
    // Czekamy aż dostaniemy wszystkie 3 lub do czasu timeoutu
    while (how_many < 3)
    {
        // Czekanie maksymalnie x sekund na pakiet w gnieździe sockfd
        ready = select (sockfd+1, &descriptors, NULL, NULL, &tv);
        if (ready < 0) 
        {
            //throw runtime_error("Select error");
            break;
        }
        if (ready == 0)
        { 
            //timeout
            break;
        }
        else
        {
            packet = get_packet(sockfd, tvalBefore);
            // Jeżeli to nasz oczekiwany pakiet
            if ((packet.id == process_id) && (packet.ttl == ttl))
            {
                packets[how_many] = packet;
                how_many++;
            }
        }
    }
    // Wypisywanie
    if (how_many == 0)
    {
        cout << ttl << ". *" <<endl; return 0;
    }
    else if (how_many == 3)
    {
        float time = (packets[0].time + packets[1].time + packets[2].time)/3000.0;
        cout << ttl << ". " << packets[0].ip;
        if ((packets[1].ip !=  packets[0].ip) && (packets[1].ip !=  packets[2].ip))
            {cout << " " << packets[1].ip;}
        if ((packets[2].ip !=  packets[0].ip) && (packets[2].ip !=  packets[1].ip)) 
            {cout << " " << packets[2].ip;}
        cout << " " << time << " ms" <<endl;
        if (packets[0].dest) {return 1;} else {return 0;} // Kończymy jeżeli osiągamy cel
    }
    else
    {
        cout << ttl << ". " << packets[0].ip;
        if (( how_many == 2 ) && (packets[1].ip !=  packets[0].ip))
            {cout << " " << packets[1].ip;}
        cout << " ??" <<endl;
        if (packets[0].dest) {return 1;} else {return 0;}  // Kończymy jeżeli osiągamy cel
    }
}
