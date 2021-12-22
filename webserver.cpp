//#ifndef webserver.h
//#define webserver.h

#include <iostream>
#include <fstream>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sstream>
#include <map>
#include <ctime>

/* 
	class Socket
		has an address that is bound to it
	
	class Server
		listens
		sends response

*/
class Socket {
	private:
		const int socketId;
		const struct sockaddr_in sAddress;

		struct sockaddr_in socketAddress(int PORT) {
			struct sockaddr_in address;

			memset((char*)&address, 0, sizeof(address));
			address.sin_family = AF_INET;
			address.sin_addr.s_addr = htonl(INADDR_ANY);
			address.sin_port = htons(PORT);

			if (bind(socketId, (const struct sockaddr*)&address, sizeof(address)) < 0) exit(0);

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

enum RequestType {
    GET,
    POST,
    DELETE,
    PUT,
    NONE
};

enum ResponsePart {
    Version,
    StatusCode,
    Date,
    ServerInfo,
    ContentType,
    ContentLength,
    Content
};

class Server {
    public:
        static void run(int port = 8080, int backlog = 3) {

            // Variable initializations
            Socket socket(port);
            const struct sockaddr_in address = socket.getAddress();
            const int adrsLen = sizeof(address), socketId = socket.getId();
            int listenSocket, bytesRead; 
            char clientPackage[30000]; // buffer for client request
            std::string package;

            // Server listens for sockets on it's port
            if (listen(socketId, backlog) < 0) exit(0); // start listening on port, backlog is max waiting requests

            while (1) {
                std::cout << "\n++++++++++ Waiting for Connection +++++++++++\n\n";

                listenSocket = accept(socketId, (struct sockaddr*)&address, (socklen_t*)&adrsLen);
                if (listenSocket < 0) exit(0);
                std::cout << "Client's socket has been accepted\n" << std::endl;

                bytesRead = read(listenSocket, clientPackage, 30000);
                if (bytesRead < 0) exit(0);
                std::cout << "Clients package has been read from the socket\n" << std::endl;

                package = buildPackage(clientPackage);
                const char *pPackage = package.c_str();

                write(listenSocket, pPackage, package.length());

                std::cout << "\n+++++++++++++++ Response Sent +++++++++++++++\n\n" << std::endl;
				std::cout << "+++++++++++++++++++++++++++++++++++++++++++++\n" << std::endl;
                close(listenSocket);
            }
        }
        // make it such that this static function cannot be called separately
        static std::string buildPackage(char cPackage[]) {
                std::string sPackage; // build package and send it out
                std::cout << "Client request is:\n\n" << cPackage << std::endl;
                //std::cout << "Length of cPackage is: " << strlen(cPackage) << std::endl;
                // Must analyze the request, see what media they are asking for
                // Build a proper response
                
                // want the first 3 words of the client package
                RequestType requestType;
                std::string header, req[3], media, mediaType;
                bool mediaTypeSave = false;
                int count = 0;
                for (int i = 0; i < strlen(cPackage); i++) {
                    if (cPackage[i] == ' ') count++;
                    else req[count] += cPackage[i];
                    if (cPackage[i] == ' ') mediaTypeSave = true;
                    if (count == 1 && mediaTypeSave) mediaType += cPackage[i];
                    if (count > 2) break;
                    
                    header += cPackage[i];
                }

                // req[0] holds the request type
                if (req[0] == "GET")
                    requestType = GET;
                else if (req[0] == "POST")
                    requestType = POST;
                else if (req[0] == "DELETE")
                    requestType = DELETE;
                else if (req[0] == "PUT")
                    requestType = PUT;
                else requestType = NONE;

                // req[1] holds the media requested
                media = req[1];

                if (requestType == NONE) return "This is not a proper HTTP request";

                sPackage = packageBuilder(requestType, media, mediaType); // takes request type (enum), media requested, user info
                return sPackage;
        }
        static std::map<ResponsePart, std::string> setUpDetails(RequestType req, std::string reqMedia, std::string mediaType, std::string user = "") {
            
            std::map<ResponsePart, std::string> md; // message details

            md[Version] = "HTTP/1.1 ";
            md[StatusCode] = "";
            md[Date] = "";
            md[ServerInfo] = "Some secret server";
            md[ContentType] = "";
            md[ContentLength] = "";
            md[Content] = "";

            // statusCode, contentType, and content are made in switch statement
            if (!auth(user)) md[StatusCode] = "401 Unauthorized"; // not available to user
            else {
                switch (req) {
                    case GET:
                        std::cout << "GET request" << std::endl;
                        // look for media
                        md[Content] = getMedia(reqMedia);
                        md[ContentType] = getMediaType(reqMedia);

                        if (md[Content].empty()) 
                        {
                            md[Content] = "No media present";
                            md[StatusCode] = "404 Not Found"; 
                        }
                        else md[StatusCode] = "200 OK";
                        break;

                    case POST:
                        std::cout << "POST request" << std::endl;
                        // add media
                        break;

                    case DELETE:
                        std::cout << "DELETE request" << std::endl;
                        // delete media
                        break;

                    case PUT:
                        std::cout << "PUT request" << std::endl;
                        break; 
                    default:
                        break;

                }
            }
            
            time_t currTime(0);
            std::stringstream cTime;
            cTime << currTime;
            std::string now = cTime.str();

            md[Date] = now;
            md[ContentLength] = md[Content].length();

            return md;
        }
        static std::string getMediaType(std::string media) {
            if (media == "rtf") 
            {
                std::cout << "RTF Media" << std::endl;
                media = "text/rtf";
            }
            else if (media == "txt")
            {
                std::cout << "TXT Media" << std::endl;
                media = "text/txt";
            }
            else if (media == "html")
            {
                std::cout << "HTML Media" << std::endl;
                media = "text/html";
            }
            return media;
        }
        static std::string packageBuilder(RequestType req, std::string media, std::string mediaType, std::string user = "") {
            std::string status, contents, package, contentType;
            std::ostringstream packageBuild;

            std::map<ResponsePart, std::string> reqDetails = setUpDetails(req, media, mediaType, user); 

            packageBuild << reqDetails[Version] 
                         << reqDetails[StatusCode]
                         << "\nDate: "
                         << reqDetails[Date]
                         << "\nServer: "
                         << reqDetails[ServerInfo]
                         << "\nContent-Type: " 
                         << reqDetails[ContentType] 
                         << "\nContent-Length: " 
                         << reqDetails[ContentLength] 
                         << "\n\n" 
                         << reqDetails[Content];

            package = packageBuild.str();
            std::cout << "\nPackage to send is:\n" << package << "\n" << std::endl;
            return package;
        }
        static bool auth(std::string u) {
            return true;
        }
        //template <typename T>
        static std::string getMedia(std::string media) {
            // look for the file
            // if found, read data from the file, send back to the response to append the data
            std::string filePath = "/Users/andredebiasi/documents/";
            filePath += media;
            std::ifstream MyFile(filePath);
            std::string output, text;

            std::cout << "Looking for file: " << media << "\n" << std::endl; 
            
            while (getline (MyFile, text)) {
                output += text;
            }

            MyFile.close();
            return output;
        }
};

int main() {
	Server::run();

	return 0;
}

//#endif