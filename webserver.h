#include <iostream>
#include <netinet/in.h>

class Socket {
    private:
        const int socketId;
        const struct sockaddr_in sAddress;

        struct sockaddr_in socketAddress(int PORT);

    public:
        Socket(int PORT);
        int getId();
        struct sockaddr_in getAddress();
};

class Server {
    public:
        static void run(int port = 8080, int backlog = 3);
        static std::string buildPackage(char cPackage[]);
};
