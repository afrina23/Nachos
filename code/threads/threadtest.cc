// threadtest.cc 
//	Simple test case for the threads assignment.
//
//	Create two threads, and have them context switch
//	back and forth between themselves by calling Thread::Yield, 
//	to illustratethe inner workings of the thread system.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "synch.h"
#include "synchlist.h"
#include "producer.h"
#include "consumer.h"

#define MAX_PRODUCER 3
#define MAX_CONSUMER 5
//----------------------------------------------------------------------
// SimpleThread
// 	Loop 5 times, yielding the CPU to another ready thread 
//	each iteration.
//
//	"which" is simply a number identifying the thread, for debugging
//	purposes.
//----------------------------------------------------------------------
Lock* lock;
Producer *producers[MAX_PRODUCER];
Consumer *consumers[MAX_CONSUMER];
Thread *p_thread[MAX_PRODUCER];
Thread *c_thread[MAX_CONSUMER];

    
void
SimpleThread(void* which)
{
    int num;
    
    lock->Acquire();
    printf("Lock is acquired by %d\n",which);
    for (num = 0; num < 5; num++) {
	printf("*** thread %d looped %d times\n",which, num);
        currentThread->Yield();
    }
    lock->Release();
    printf("Lock is released by %d\n",which);
}
void produce(void* p){
    int no=(long) p;
    producers[no]->produce();
}    

void consume(void* c){
    int no=(long) c;
    consumers[no]->consume();
   
}
//----------------------------------------------------------------------
// ThreadTest
// 	Set up a ping-pong between two threads, by forking a thread 
//	to call SimpleThread, and then calling SimpleThread ourselves.
//----------------------------------------------------------------------

void producer_consumer(){
    SynchList<int> *common_queue= new SynchList<int>; 
    DEBUG('t', "Creating  producers\n");
    
    for(int i=0;i<MAX_PRODUCER;i++){
        
        producers[i]= new Producer(i+1,common_queue);
        char* name = (char *)malloc(sizeof(char) * 16);
        sprintf(name,"Producer Thread %d",i+1);
        p_thread[i]= new Thread(name);
        DEBUG('t', "Starting producer\n");
        p_thread[i]->Fork(produce,(void*)i);
        DEBUG('t', "producer is forked\n");
    }
       
    
    for(int i=0;i<MAX_CONSUMER;i++){
        consumers[i]= new  Consumer(i+1,common_queue);
        char* name = (char *)malloc(sizeof(char) * 16);
        sprintf(name,"Consumer Thread %d",i+1);
        c_thread[i]= new Thread(name);
        DEBUG('t', "Starting consumer\n");
        c_thread[i]->Fork(consume,(void*)i);
        DEBUG('t', "consumer is forked\n");
        
    }
    
}    

void
ThreadTest()
{
    DEBUG('t', "Entering producerConsumer\n");
   // producer_consumer();
    
     DEBUG('t', "Entering SimpleTest");
     
     lock= new Lock("number lock");
 
    Thread *t1 = new Thread("forked thread 1");
     Thread *t2 = new Thread("forked thread 2");
     t1->Fork(SimpleThread,(void*)1);
     t2->Fork(SimpleThread,(void*)2);
      SimpleThread(0);
}


