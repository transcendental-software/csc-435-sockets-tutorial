#ifndef WORKER_H
#define WORKER_H

#include <iostream>

class Worker
{
    int sock;

    static const int MAX_BUFFER_SIZE = 2048;

    public:
        Worker(int sock) : sock(sock) { }
        virtual ~Worker() = default;

        virtual void run();
};

#endif