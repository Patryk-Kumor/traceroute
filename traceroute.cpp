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
    // Struct adressu ip
    struct sockaddr_in address;
    // Zerowanie bitów na całym strucie
    bzero(&address, sizeof(address)); 
    // AF_INET
    address.sin_family = AF_INET;
    // Przekształca wartość z lokalnego na sieciowy porządek bajtów.
    address.sin_port = htons (0); 
    // Konwertuje IP z tekstu do formy binarnej, jeśli się nie uda (ip jest nieporpawne) - zwraca 0
    if (inet_pton(AF_INET, argv[1], &address.sin_addr) == 0)
    {
       throw runtime_error("IP error\n"); 
    }
    // Unikalny id procesu w celu identyfikacji requestów i odpowiedzi
    int process_id = getpid();
    // Główny traceroute
    for (int ttl = 1; ttl <= 30; ttl++)
    {
        // Wysyłanie 3 requestów o tym samym ttlu
        send(sockfd, ttl, process_id, address);
        // Oznaczenie czasu
        struct timeval tvalBefore;
        gettimeofday (&tvalBefore, NULL);
        // Odbieranie odpowiedzi
        if (receive(sockfd, ttl, process_id, tvalBefore)) { break; }
    }
    // Koniec pracy
    return 0;
}
catch (const exception& e) 
{
    cout << ">> Program zamknięto <<\nerror: " << e.what() << "\n";
    return -1;
}
