

#include "copyright.h"
#include "synchlist.h"
#include "thread.h"
#include "system.h"
#include "consumer.h"


Consumer::Consumer(int id,SynchList<int> *q){
    
    consumer_id=id;
    queue=q;
  //  consumer_thread = new Thread((const char*)consumer_id);
   // Fork(consume);
    
}    

Consumer::~Consumer(){
    delete queue;
  //  delete consumer_thread;
    
}

void Consumer::consume(){
    for(int i=1;i<=5;i++){
        int consumed=queue->Remove();
        printf("Consumer %d has consumed %d\n",consumer_id,consumed);
        currentThread->Yield();
        for(int j=0;j<500;j++){
            //printf("Consumer %d is waiting after consuming\n",consumer_id);
        }
    }    
}    
    

