

#include "copyright.h"
#include "synchlist.h"
#include "thread.h"
#include "system.h"
#include "producer.h"


int Producer::produced_number=1;
Producer::Producer(int id,SynchList<int> *q){
    
    producer_id=id;
    queue=q;
   // producer_thread = new Thread((const char*)producer_id);
    //producer_thread->Fork(this->produce);
   // Fork(produce);
    
}    

Producer::~Producer(){
    delete queue;
    //delete producer_thread;
    
}

void Producer::produce(){
    for(int i=1;i<=5;i++){
        IntStatus oldLevel = interrupt->SetLevel(IntOff);	// disable interrupts
        queue->Append(produced_number);
        printf("Producer %d has produced %d\n",producer_id,produced_number);
        produced_number++;
        interrupt->SetLevel(oldLevel);
        
        currentThread->Yield();
        for(int j=0;j<500;j++){
           // printf("Producer %d is waiting after producing \n",producer_id);
        }
    }    
}    
    

