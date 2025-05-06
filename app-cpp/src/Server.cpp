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
    bool terminate;

    public:
        // initialize the address so that the server will listen to all interfaces
        Server(std::string port, int maxNumConnections) : 
            address("0.0.0.0"), port(port), maxNumConnections(maxNumConnections), terminate(false) { }
        virtual ~Server() = default;

        virtual void setTerminate()
        {
            terminate = true;
        }

        virtual void run()
        {
            int server_sockfd, client_sockfd;
            struct addrinfo hints;
            struct addrinfo* servinfo;
            struct addrinfo* p;
            struct sockaddr_storage clt_addr;
            socklen_t sin_size;
            char addr[INET6_ADDRSTRLEN];
            int yes = 1;
            int rc;

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
                if ((server_sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
                    std::cerr << "Could not create socket!" << std::endl;
                    continue;
                }

                // configure server socket
                if (setsockopt(server_sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
                    std::cerr << "Could not configure socket!" << std::endl;
                    return;
                }

                // bind server socket
                if (bind(server_sockfd, p->ai_addr, p->ai_addrlen) == -1) {
                    close(server_sockfd);
                    std::cerr << "Could not bind socket!" << std::endl;
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

            // listen for connections
            if (listen(server_sockfd, maxNumConnections) == -1) {
                std::cerr << "Server failed to listen!" << std::endl;
                return;
            }

            std::cout << "Server started and waiting for connections!" << std::endl;
            std::cout << "> " << std::flush;

            while(true) {
                sin_size = sizeof(clt_addr);

                // accept connection from client
                client_sockfd = accept(server_sockfd, (struct sockaddr *)&clt_addr, &sin_size);
                if (client_sockfd == -1) {
                    std::cerr << "Server failed to accept client!" << std::endl;
                    continue;
                }

                if (terminate) {
                    close(client_sockfd);
                    break;
                }

                // extract client information
                if (((struct sockaddr *) &clt_addr)->sa_family == AF_INET) {
                    inet_ntop(clt_addr.ss_family, &(((struct sockaddr_in*) &clt_addr)->sin_addr), addr, sizeof(addr));
                } else {
                    inet_ntop(clt_addr.ss_family, &(((struct sockaddr_in6*) &clt_addr)->sin6_addr), addr, sizeof(addr));
                }

                std::cout << std::endl << "Server got connection from " << addr << "!";

                // create worker thread for new client connection
                workers.push_back(Worker(client_sockfd));
                threads.push_back(std::thread(&Worker::run, &(workers.back())));
            }

            close(server_sockfd);

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
    std::thread serverThread = std::thread(&Server::run, &server);

    std::string command;
    while (true) {
        std::getline(std::cin, command);

        if (command.compare("quit") == 0) {
            int sockfd;
            struct addrinfo hints;
            struct addrinfo* servinfo;
            struct addrinfo* p;
            char addr[INET6_ADDRSTRLEN];
            int rc;

            server.setTerminate();

            // specify server address properties
            memset(&hints, 0, sizeof(hints));
            hints.ai_family = AF_UNSPEC;
            hints.ai_socktype = SOCK_STREAM;

            // get server address information
            if ((rc = getaddrinfo("127.0.0.1", port.c_str(), &hints, &servinfo)) != 0) {
                std::cerr << "Could not get address information!" << std::endl;
                return 1;
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
                return 1;
            }

            // release server information data structure
            freeaddrinfo(servinfo);

            close(sockfd);
            serverThread.join();
            
            std::cout << "Server terminated!" << std::endl;
            break;
        }
    }
    
    return 0;
}