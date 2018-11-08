#ifndef CONSUMER_H
#define CONSUMER_H

#include "copyright.h"
#include "synchlist.h"
#include "thread.h"
class Consumer{
public:
    Consumer(int id,SynchList<int> *q);
    ~Consumer();
    int getId() { return consumer_id;}
    void consume();
    
private:
    int consumer_id;
    SynchList<int> *queue;
    //Thread* consumer_thread;
    
};


#endif