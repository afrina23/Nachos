#ifndef PRODUCER_H
#define PRODUCER_H

#include "copyright.h"
#include "synchlist.h"
#include "thread.h"
class Producer{
public:
    Producer(int id,SynchList<int> *q);
    ~Producer();
    int getId() { return producer_id;}
    void produce();
    
private:
    int producer_id;
    SynchList<int> *queue;
    static int produced_number;
    //Thread* producer_thread;
    
};


#endif 