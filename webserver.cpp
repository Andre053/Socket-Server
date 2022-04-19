#include "server.hpp"


// takes command line arguments
// argc : count of arguments, includes the name of the file
// argv : array of arguments passed
int main(int argc, char *argv[]) {

	if (argc < 3) return 0;
	int PORT = atoi(argv[1]), BACKLOG = atoi(argv[2]);

	std::cout << "Port: " << PORT << "\nBacklog: " << BACKLOG << std::endl;

	std::cout << "Inet: " << AF_INET << std::endl;

	Server serverInstance(PORT, BACKLOG);
	serverInstance.run();

	return 0;


}



















