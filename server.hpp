#include <unistd.h> // gives access to: read, write, close
#include <ctime> // gives access to current time

// Research inet protocols and lower level workings
#include <arpa/inet.h> // gives access to inet_ntop/pton - converts binary address to string or vice versa
#include "socket.hpp"
#include <typeinfo> // type checking for command line inputs


class Server {
    private:
        Socket socket;
        const struct sockaddr_in address;
        const int adressLen, socketId;
        int listenSocket, bytesRead, backlog;
        char clientPackage[30000];
        std::string package;
    
    public:
        Server() : 
            socket(8080),
            address(socket.getAddress()),
            adressLen(sizeof(address)),
            socketId(socket.getId()),
            backlog(3)
        {}
        Server(int port_num, int backlogMax) :
            socket(port_num),
            address(socket.getAddress()),
            adressLen(sizeof(address)),
            socketId(socket.getId()),
            backlog(backlogMax)
        {}

        void run() {
            listenLoop();
        }

        void listenLoop() {
            
            if (listen(socketId, backlog) < 0) exit(0); // start listening on port, backlog is max waiting requests
            
            std::cout 
                    << "\nSocker ID: " 
                    << this->socketId
                    << std::endl;
            while (1) {
                std::cout << "\n++++++++++ Waiting for Connection +++++++++++\n\n";
                
                listenSocket = accept(socketId, (struct sockaddr*)&address, (socklen_t*)&adressLen);
                if (listenSocket < 0) exit(0);
                getClientInfo(address);
                std::cout << "Client's socket has been accepted\n" << std::endl;

                bytesRead = read(listenSocket, clientPackage, 30000);
                if (bytesRead < 0) exit(0);
                std::cout << "Clients package has been read from the socket\n" 
                    << clientPackage
                    << std::endl;

                //package = buildPackage(clientPackage);
                //const char *pPackage = package.c_str();

                //write(listenSocket, pPackage, package.length());

                //std::cout << "\n+++++++++++++++ Response Sent +++++++++++++++\n\n" << std::endl;
				std::cout << "+++++++++++++++++++++++++++++++++++++++++++++\n" << std::endl;
                close(listenSocket);
            }}

        void getClientInfo(struct sockaddr_in addrss) {
            char ip[INET_ADDRSTRLEN];
            // binary address to character array "ip"
            inet_ntop(AF_INET, &(address.sin_addr), ip, INET_ADDRSTRLEN);
            int port = (int)ntohs(address.sin_port);
            std::cout << "Client IP: " << ip << "\nClient Port: " << port << std::endl;
        }
};