#include <unistd.h> // gives access to: read, write, close
#include <ctime> // gives access to current time

// Research inet protocols and lower level workings
#include <arpa/inet.h> // gives access to inet_ntop/pton - converts binary address to string or vice versa
#include "socket.hpp"
#include <typeinfo> // type checking for command line inputs
#include <unordered_map>
#include <ctime>
#include <sstream>
#include <fstream>

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
            std::cout << "Running server" << std::endl;
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

                package = buildPackage(clientPackage);
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

        std::string buildPackage(std::string cPackage) {
            std::string sPackage; // build package and send it out
            std::cout << "Client request is:\n\n" << cPackage << std::endl;
            //std::cout << "Length of cPackage is: " << strlen(cPackage) << std::endl;
            // Must analyze the request, see what media they are asking for
            // Build a proper response
            
            // want the first 3 words of the client package
            std::string requestType;
            std::string header, req[3], media, mediaType;
            bool mediaTypeSave = false;
            int count = 0;
            for (int i = 0; i < cPackage.length(); i++) {
                if (cPackage[i] == ' ') count++;
                else req[count] += cPackage[i];
                if (cPackage[i] == ' ') mediaTypeSave = true;
                if (count == 1 && mediaTypeSave) mediaType += cPackage[i];
                if (count > 2) break;
                
                header += cPackage[i];
            }
            
            if (req[0] != "GET") 
                return "This is not an accepted HTTP request";

            requestType = req[0];
            media = req[1];

            sPackage = packageBuilder(requestType, media, mediaType); // takes request type (enum), media requested, user info
            return sPackage;
        }

        std::unordered_map<std::string, std::string> setUpDetails(std::string req, std::string reqMedia, std::string mediaType, std::string user = "") {
            
            std::unordered_map<std::string, std::string> md; // message details

            md["Version"] = "HTTP/1.1 ";
            md["StatusCode"] = "";
            md["Date"] = "";
            md["ServerInfo"] = "Some secret server";
            md["ContentType"] = "";
            md["ContentLength"] = "";
            md["Content"] = "";

            // statusCode, contentType, and content are made in switch statement
            if (!auth(user)) md["StatusCode"] = "401 Unauthorized"; // not available to user

            // look for media
            md["Content"] = getMedia(reqMedia);
            md["ContentType"] = getMediaType(reqMedia);

            if (md["Content"].empty()) 
            {
                md["Content"] = "No media present";
                md["StatusCode"] = "404 Not Found"; 
            }
            else md["StatusCode"] = "200 OK";
            
            time_t currTime(0);
            std::stringstream cTime;
            cTime << currTime;
            std::string now = cTime.str();

            md["Date"] = now;
            md["ContentLength"] = md["Content"].length();

            return md;
        }

        std::string getMedia(std::string reqMedia) {
            // look for the file
            // if found, read data from the file, send back to the response to append the data
            
            // add error check for file path
            std::string filePath = std::getenv("RESOURCE_PATH"); // base path (use environment variable)
            filePath += reqMedia;

            std::cout << "Looking for file at: " << filePath << "\n" << std::endl; 

            std::ifstream reqFile(filePath);
            std::string output, text;

            
            while (getline (reqFile, text)) {
                output += text;
            }

            reqFile.close();
            return output;
        }

        std::string getMediaType(std::string media) {
            // check file extension 
            std::string extension = media.substr(media.find_last_of(".") + 1);
            
            if (extension == "rtf") 
            {
                std::cout << "RTF Media" << std::endl;
                return "text/rtf";
            }
            else if (extension == "txt")
            {
                std::cout << "TXT Media" << std::endl;
                return "text/txt";
            }
            else if (extension == "html")
            {
                std::cout << "HTML Media" << std::endl;
                return "text/html";
            }
            else if (extension == "pdf") 
            {
                std::cout << "PDF Media" << std::endl;
                return "application/pdf";
            }
            else return "";
        }

        std::string packageBuilder(std::string reqType, std::string media, std::string mediaType, std::string user = "") {
            std::string status, contents, package, contentType;
            std::ostringstream packageBuild;

            std::unordered_map<std::string, std::string> reqDetails = setUpDetails(reqType, media, mediaType, user); 

            packageBuild << reqDetails["Version"] 
                         << reqDetails["StatusCode"]
                         << "\nDate: "
                         << reqDetails["Date"]
                         << "\nServer: "
                         << reqDetails["ServerInfo"]
                         << "\nContent-Type: " 
                         << reqDetails["ContentType"] 
                         << "\nContent-Length: " 
                         << reqDetails["ContentLength"] 
                         << "\n\n" 
                         << reqDetails["Content"];

            package = packageBuild.str();
            std::cout << "\nPackage to send is:\n" << package << "\n" << std::endl;
            return package;
        }

        bool auth(std::string u) {
            return true;
        }
};