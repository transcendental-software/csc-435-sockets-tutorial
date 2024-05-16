#include "Worker.hpp"

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

void Worker::run() {
    char* buf = new char[MAX_BUFFER_SIZE];
    int numBytes;

    while (true) {
        // receive a request message from a client
        memset(buf, 0, MAX_BUFFER_SIZE);
        if ((numBytes = recv(sock, buf, MAX_BUFFER_SIZE - 1, 0)) == -1) {
            std::cerr << "Error receiving data!" << std::endl;
            break;
        }
        
        if (strcmp(buf, "quit") == 0) {
            break;
        }

        if (strcmp(buf, "addition") == 0) {
            memset(buf, 0, MAX_BUFFER_SIZE);
            strcpy(buf, "2+2=4");
            // send reply message to the client
            if (send(sock, buf, strlen(buf), 0) == -1) {
                std::cerr << "Error sending data!" << std::endl;
                break;
            }
            continue;
        }

        if (strcmp(buf, "multiplication") == 0) {
            memset(buf, 0, MAX_BUFFER_SIZE);
            strcpy(buf, "2x2=4");
            // send reply message to the client
            if (send(sock, buf, strlen(buf), 0) == -1) {
                std::cerr << "Error sending data!" << std::endl;
                break;
            }
            continue;
        }

        // send reply message to the client
        memset(buf, 0, MAX_BUFFER_SIZE);
        strcpy(buf, "???");
        if (send(sock, buf, strlen(buf), 0) == -1) {
            std::cerr << "Error sending data!" << std::endl;
            break;
        }
    }

    delete buf;
}