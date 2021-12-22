#ifndef webserver.h
#define webserver.h

#include <iostream>
#include <fstream>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sstream>

Socket::Socket(int PORT) :
socketId(this->socketId),
sAddress(this->socketAddress(PORT))
{
}
struct sockaddr_in Socket::socketAddress(int PORT) {
	struct sockaddr_in address;

	memset((char*)&address, 0, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = htonl(INADDR_ANY);
	address.sin_port = htons(PORT);

	if (bind(this->socketId, (const struct sockaddr*)&address, sizeof(address)) < 0) exit(0);

	return address;
}

#endif