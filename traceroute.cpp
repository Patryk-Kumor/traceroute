// Patryk Kumor
// indeks: 292601
#include "traceroute.h"

int main(int argc, char* argv[]) try
{
    // Sprawdzanie liczby argumentów
    if (argc != 2)
    {
        throw runtime_error("Nieprawidłowa liczba argumentów\n");
    }

    // Tworzenie socketów wymaga sudo
    int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sockfd < 0) 
    {
        throw runtime_error("Socket error, może brakuje sudo?\n");
    }

    struct sockaddr_in address;
    bzero(&address, sizeof(address)); // zerowanie bitów na całym strucie
    address.sin_family = AF_INET; // AF_INET
    address.sin_port = htons (0); // przekształca wartość short  integer hostshort z lokalnego na sieciowy porządek bajtów.
  	if (inet_pton(AF_INET, argv[1], &address.sin_addr) == 0) // Konwertuje IP z tekstu do formy binarnej, jeśli się nie uda - zwraca 0
    {
        throw runtime_error("IP error\n");
    }

    // Główny traceroute

    int process_id = getpid();
    for (int ttl = 1; ttl <= 30; ttl++)
    {

        send(sockfd, ttl, process_id, address);

        //oznaczenie czasu
        clock_t start_time = clock();

        //czekanie
        if (receive(sockfd, ttl, process_id, start_time)) {break;}
    }

    // Koniec pracy
    return 0;
}
catch (const exception& e) 
{
    cout << ">> Program zamknięto <<\nerror: " << e.what() << "\n";
    return -1;
}


