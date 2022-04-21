#include <arpa/inet.h>
#include <iostream>
#include <fstream>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string.h> // needed for memset

#define PORT 8080 // the port we are speaking to

/*
	### Client Connection Emulator ###

	This short program simulates connecting to our web server.
	We must run in a separate terminal and we can see the interaction between client and server.

	Used this for early testing purposes, now the server has been specified to work with HTTP GET requests.

*/

int run(std::string package)
{
	
	int sock = 0; // the socket we are sending our message on
	long valread; // the result of reading the socket
	struct sockaddr_in serv_addr;
	std::string *pPackage = &package; // pointing to clients package to send
	
	char serverPackage[30000];

	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		std::cout << "Socket creation error" << std::endl;
		return -1;
	}

	memset(&serv_addr, '0', sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);

	if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
	{
		std::cout << "Invalid address" << std::endl;
		return -1;
	}

	if (connect(sock, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0)
	{
		std::cout << "Connection failed" << std::endl;
		return -1;
	}

	send(sock, pPackage, sizeof(package), 0);
	std::cout << "\nPackage sent without error" << std::endl;

	valread = read(sock, serverPackage, 30000);

	//std::cout << "\nChar ptr is:  " << pServerPackage << std::endl;
	//std::cout << "\nChar ptr dereferenced is:  " << *pServerPackage << std::endl;
	std::cout << "\nVal read is: " << valread << std::endl;
	std::cout << "\nString is:\n" << serverPackage << std::endl;
	
	return 0;
}


int main()
{
	std::string request;
	std::getline(std::cin, request);
	return run(request);
}
