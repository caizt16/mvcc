#include <cstdio>
#include "mvcc.h"
#include <ctime>
#include <iostream>

int main(int argc, const char *argv[]){
    int threads = 0;
    if(argc >= 2)
        sscanf(argv[1], "%d", &threads);
    else
        threads = 1;
    MVCC* mvcc = new MVCC(threads);
    mvcc->run();
    mvcc->print();
    delete mvcc;
    return 0;
}