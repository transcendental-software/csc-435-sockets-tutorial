#include <iostream>

extern "C"
{
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <unistd.h>
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <netdb.h>
    #include <arpa/inet.h>
}

#include <vector>
#include <string>
#include <sstream>

class Client
{
    int clientID;
    std::string address;
    std::string port;

    static const int MAX_BUFFER_SIZE = 2048;

    public:
        Client(int clientID, std::string address, std::string port) :
            clientID(clientID), address(address), port(port) { }
        virtual ~Client() = default;

        virtual void run()
        {
            int sockfd;
            struct addrinfo hints;
            struct addrinfo* servinfo;
            struct addrinfo* p;
            char addr[INET6_ADDRSTRLEN];
            int rc;

            char* buf = new char[MAX_BUFFER_SIZE];
            int numBytes;
            std::string data;

            // specify server address properties
            memset(&hints, 0, sizeof(hints));
            hints.ai_family = AF_UNSPEC;
            hints.ai_socktype = SOCK_STREAM;

            // get server address information
            if ((rc = getaddrinfo(address.c_str(), port.c_str(), &hints, &servinfo)) != 0) {
                std::cerr << "Could not get server address information!" << std::endl;
                return;
            }

            for(p = servinfo; p != NULL; p = p->ai_next) {
                // create client socket
                if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
                    std::cerr << "Could not create socket!" << std::endl;
                    continue;
                }

                // connect client to server
                if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
                    close(sockfd);
                    std::cerr << "Could not connect to server!" << std::endl;
                    continue;
                }

                break;
            }

            if (p == NULL)  {
                std::cerr << "Fatal Error: Ran out of connection options!" << std::endl;
                // release server information data structure
                freeaddrinfo(servinfo);
                return;
            }

            // release server information data structure
            freeaddrinfo(servinfo);

            // extract server information
            if (((struct sockaddr *) &p)->sa_family == AF_INET) {
                inet_ntop(p->ai_family, &(((struct sockaddr_in*) &p->ai_addr)->sin_addr), addr, sizeof(addr));
            } else {
                inet_ntop(p->ai_family, &(((struct sockaddr_in6*) &p->ai_addr)->sin6_addr), addr, sizeof(addr));
            }

            std::cout << "Client connected to " << address << "!" << std::endl;

            // send INDEX request message to server (worker)
            data = "INDEX " + std::to_string(clientID) + " DOC11 tiger 100 cat 10 dog 20";
            memset(buf, 0, MAX_BUFFER_SIZE);
            strcpy(buf, data.c_str());
            if (send(sockfd, buf, strlen(buf), 0) == -1) {
                std::cerr << "Error sending data!" << std::endl;
                delete buf;
                close(sockfd);
                return;
            }

            // receive INDEX reply message from server (worker)
            memset(buf, 0, MAX_BUFFER_SIZE);
            if ((numBytes = recv(sockfd, buf, MAX_BUFFER_SIZE - 1, 0)) == -1) {
                std::cerr << "Error receiving data!" << std::endl;
                delete buf;
                close(sockfd);
                return;
            }
            data = std::string(buf);
            std::cout << "Indexing " << data << std::endl;

            // send SEARCH request message to server (worker)
            data = "SEARCH cat";
            memset(buf, 0, MAX_BUFFER_SIZE);
            strcpy(buf, data.c_str());
            if (send(sockfd, buf, strlen(buf), 0) == -1) {
                std::cerr << "Error sending data!" << std::endl;
                delete buf;
                close(sockfd);
                return;
            }

            // receive SEARCH reply message from server (worker)
            memset(buf, 0, MAX_BUFFER_SIZE);
            if ((numBytes = recv(sockfd, buf, MAX_BUFFER_SIZE - 1, 0)) == -1) {
                std::cerr << "Error receiving data!" << std::endl;
                delete buf;
                close(sockfd);
                return;
            }
            data = std::string(buf);
            std::string token;
            std::vector<std::string> tokens;
            std::stringstream message_stream(data);
            std::cout << "Searching for cat" << std::endl;
            while (std::getline(message_stream, token, ' ')) {
                tokens.push_back(token);
            }
            for (auto i = 0; i < tokens.size(); i += 2) {
                std::cout << tokens[i] << " " << tokens[i + 1] << std::endl;
            }

            // send QUIT message to server (worker)
            data = "QUIT";
            memset(buf, 0, MAX_BUFFER_SIZE);
            strcpy(buf, data.c_str());
            if (send(sockfd, buf, strlen(buf), 0) == -1) {
                std::cerr << "Error sending data!" << std::endl;
                delete buf;
                close(sockfd);
                return;
            }
            
            // close connection
            delete buf;
            close(sockfd);
        }
};

int main(int argc, char** argv)
{
    if (argc != 4) {
        std::cerr << "USE: ./client <client ID> <IP address> <port>" << std::endl;
        return 1;
    }

    std::string clientID(argv[1]);
    std::string address(argv[2]);
    std::string port(argv[3]);

    Client client(std::stoi(clientID), address, port);
    client.run();

    return 0;
}