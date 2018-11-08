#include "copyright.h"
#include "processTable.h"
#include "synch.h"

bool ProcessTable::createInstance = false;
ProcessTable* ProcessTable::processTable = NULL;


ProcessTable* ProcessTable::getInstance(int Size)
{
    if(!createInstance)
    {
        processTable = new ProcessTable(Size);
        createInstance = true;
        return processTable;
    }
    else
    {
        return processTable;
    }
}
ProcessTable::ProcessTable(int size){
    processArray= new void*[size+1];
    for(int i=1;i<=size;i++){
            processArray[i]=NULL;
    }
    processTableLock= new Lock("Process Table Lock");
    no_of_processes=0;
    max_processes=size;
}

int ProcessTable::Alloc(void * process){
    int to_return=-1;
    processTableLock->Acquire();
    for(int i=1;i<=max_processes;i++){
        if(processArray[i]==NULL){
            to_return=i;
            processArray[i]=process;
            no_of_processes++;
            break;
        }
    }
    processTableLock->Release();
    return to_return;
}
void* ProcessTable::Get(int index){
    void* to_return=NULL;
    processTableLock->Acquire();
    to_return=processArray[index];
    processTableLock->Release();
    return to_return;
}
void ProcessTable::Release(int index){
    processTableLock->Acquire();
   // if(processArray[index]!= NULL){
        printf("Index %d\n",index);
        no_of_processes--;
   // }
    processArray[index]=NULL;

    processTableLock->Release();
}
