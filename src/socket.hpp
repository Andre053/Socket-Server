#include <iostream>
#include <netinet/in.h> // gives access to listen, accept, address, AF_INET, INADDR_ANY, etc.
#include <string.h> // gives memset: sets memory at pointer location
#include <cstdlib> // getenv function

/*
    ### Socket Class ###

    Basic class to initialize a TCP/IP socket
    Will make it such that it will look for an IP address environment variable, if not found will connect to any address
*/

class Socket {
    private:
        const int socketId;
        const struct sockaddr_in sAddress;

        struct sockaddr_in socketAddress(int PORT) {
            struct sockaddr_in address;

            std::cout << "Creating socket" << std::endl;

            char* ipAddress;
            // IP address is set as an environment variable
            // std::string ipVariable = std::getenv("IP");
            // ipAddress = &ipVariable[0];

            memset((char*)&address, 0, sizeof(address));

            // Internet address is the family
            address.sin_family = AF_INET;

            // htonl(INADDR_ANY) should connect with all available interfaces, some implementations only connect with first
            // inet_addr("127.0.0.1")
            // address.sin_addr.s_addr = inet_addr(ipAddress); // can use IP address from environment variable
            address.sin_addr.s_addr = inet_addr("127.0.0.1");

            std::cout << "Internet address: " << address.sin_addr.s_addr << std::endl;

            // bind socket to the given port
            address.sin_port = htons(PORT);

            if (bind(socketId, (const struct sockaddr*)&address, sizeof(address)) < 0) exit(0);
            std::cout << "Socket binded" << std::endl;
            return address;
        }

    public:
        Socket(int PORT) :
            socketId(socket(AF_INET, SOCK_STREAM, 0)),
            sAddress(this->socketAddress(PORT))
            {}

        int getId() {
            return this->socketId;
        }
        struct sockaddr_in getAddress() {
            return this->sAddress;
        }
};