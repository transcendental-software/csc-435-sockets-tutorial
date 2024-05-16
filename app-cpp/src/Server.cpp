#include <iostream>
#include <string>
#include <vector>
#include <thread>

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

#include "Worker.hpp"

class Server
{
    std::string address;
    std::string port;
    int maxNumConnections;

    public:
        // initialize the address so that the server will listen to all interfaces
        Server(std::string port, int maxNumConnections) : 
            address("0.0.0.0"), port(port), maxNumConnections(maxNumConnections) { }
        virtual ~Server() = default;

        virtual void run()
        {
            int sockfd, client_sock;
            struct addrinfo hints;
            struct addrinfo* servinfo;
            struct addrinfo* p;
            struct sockaddr_storage clt_addr;
            socklen_t sin_size;
            char addr[INET6_ADDRSTRLEN];
            int yes = 1;
            int rc;

            // configure server to close after 2 clients connected
            int i = 2;

            std::vector<Worker> workers;
            std::vector<std::thread> threads;

            // specify server address properties
            memset(&hints, 0, sizeof(hints));
            hints.ai_family = AF_UNSPEC;
            hints.ai_socktype = SOCK_STREAM;
            hints.ai_flags = AI_PASSIVE;

            // get server address information
            if ((rc = getaddrinfo(address.c_str(), port.c_str(), &hints, &servinfo)) != 0) {
                std::cerr << "Could not get address information!" << std::endl;
                return;
            }

            for(p = servinfo; p != NULL; p = p->ai_next) {
                // create server socket
                if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
                    std::cerr << "Could not create socket!" << std::endl;
                    continue;
                }

                // configure server socket
                if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
                    std::cerr << "Could not configure socket!" << std::endl;
                    return;
                }

                // bind server socket
                if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
                    close(sockfd);
                    std::cerr << "Could not bind socket!" << std::endl;
                    continue;
                }

                break;
            }

            // release server information data structure
            freeaddrinfo(servinfo);

            if (p == NULL)  {
                std::cerr << "Server failed to bind!" << std::endl;
                return;
            }

            // listen for connections
            if (listen(sockfd, maxNumConnections) == -1) {
                std::cerr << "Server failed to listen!" << std::endl;
                return;
            }

            std::cout << "Server started and waiting for connections!" << std::endl;

            while(i > 0) {
                sin_size = sizeof(clt_addr);

                // accept connection from client
                client_sock = accept(sockfd, (struct sockaddr *)&clt_addr, &sin_size);
                if (client_sock == -1) {
                    std::cerr << "Server failed to accept client!" << std::endl;
                    continue;
                }

                // extract client information
                if (((struct sockaddr *) &clt_addr)->sa_family == AF_INET) {
                    inet_ntop(clt_addr.ss_family, &(((struct sockaddr_in*) &clt_addr)->sin_addr), addr, sizeof(addr));
                } else {
                    inet_ntop(clt_addr.ss_family, &(((struct sockaddr_in6*) &clt_addr)->sin6_addr), addr, sizeof(addr));
                }

                std::cout << "Server got connection from " << addr << "!" << std::endl;

                // create worker thread for new client connection
                workers.push_back(Worker(client_sock));
                threads.push_back(std::thread(&Worker::run, &(workers.back())));
                i--;
            }

            for (int i = 0 ; i < threads.size(); i++) {
                threads[i].join();
            }
        }
};

int main(int argc, char** argv)
{
    if (argc != 2) {
        std::cerr << "USE: ./server <port>" << std::endl;
        return 1;
    }

    std::string port(argv[1]);

    Server server(port, 4);
    server.run();
    
    return 0;
}