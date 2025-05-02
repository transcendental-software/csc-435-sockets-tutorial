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

#include <vector>
#include <string>
#include <sstream>

void Worker::run() {
    char* buf = new char[MAX_BUFFER_SIZE];
    int numBytes;

    while (true) {
        memset(buf, 0, MAX_BUFFER_SIZE);
        if ((numBytes = recv(sock, buf, MAX_BUFFER_SIZE - 1, 0)) == -1) {
            std::cerr << "Error receiving data!" << std::endl;
            break;
        }
        std::string message = std::string(buf);

        if (message.compare("QUIT") == 0) {
            break;
        }

        if (message.substr(0, 5).compare("INDEX") == 0) {
            std::string token;
            std::vector<std::string> tokens;
            std::stringstream message_stream(message);
            while (std::getline(message_stream, token, ' ')) {
                tokens.push_back(token);
            }

            std::cout << std::endl << "indexing " << tokens[2] << " from " << tokens[1] << std::endl;
            for (auto i = 3; i < tokens.size(); i += 2) {
                std::cout << tokens[i] << " " << tokens[i + 1] << std::endl;
            }
            std::cout << "completed!" << std::endl;

            std::string data{"OK"};
            memset(buf, 0, MAX_BUFFER_SIZE);
            strcpy(buf, data.c_str());
            if (send(sock, buf, strlen(buf), 0) == -1) {
                std::cerr << "Error sending data!" << std::endl;
                break;
            }

            std::cout << "> " << std::flush;
            
            continue;
        }

        if (message.substr(0, 6).compare("SEARCH") == 0) {
            std::string token;
            std::vector<std::string> tokens;
            std::stringstream message_stream(message);
            while (std::getline(message_stream, token, ' ')) {
                tokens.push_back(token);
            }

            std::cout << std::endl << "searching for " << tokens[0] << std::endl;

            std::string data{"DOC10 20 DOC100 30 DOC1 10"};
            memset(buf, 0, MAX_BUFFER_SIZE);
            strcpy(buf, data.c_str());
            if (send(sock, buf, strlen(buf), 0) == -1) {
                std::cerr << "Error sending data!" << std::endl;
                break;
            }

            std::cout << "> " << std::flush;
            
            continue;
        }

        std::string data{"ERROR"};
        memset(buf, 0, MAX_BUFFER_SIZE);
        strcpy(buf, data.c_str());
        if (send(sock, buf, strlen(buf), 0) == -1) {
            std::cerr << "Error sending data!" << std::endl;
            break;
        }
    }

    std::string data{"TERMINATE"};
    memset(buf, 0, MAX_BUFFER_SIZE);
    strcpy(buf, data.c_str());
    if (send(sock, buf, strlen(buf), 0) == -1) {
        std::cerr << "Error sending data!" << std::endl;
    }

    delete buf;
    close(sock);
}