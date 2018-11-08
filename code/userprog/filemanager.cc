#include "copyright.h"
#include "system.h"
#include "filemanager.h"
#include "synch.h"
#include "translate.h"
#include "processTable.h"

bool MemoryManager::createInstance = false;
MemoryManager* MemoryManager::manager = NULL;
extern ProcessTable* processTable;

MemoryManager* MemoryManager::getInstance()
{
    if(!createInstance)
    {
        manager = new MemoryManager(NumPhysPages);
        createInstance = true;
        return manager;
    }
    else
    {
        return manager;
    }
}


MemoryManager::MemoryManager(int numPages){
    maxPages=numPages;
    pageUsed=0;
    memoryMap= new BitMap(numPages);
    memoryLock= new Lock("memory Manager Lock");
    processMap= new int[numPages];
    entries= new TranslationEntry*[numPages];
}
MemoryManager::~MemoryManager(){
    delete memoryMap;
    delete memoryLock;
    delete processMap;
    delete [] entries;
}
int MemoryManager::AllocPage(){
    if(pageUsed>= maxPages){
        return -1;
    }
    memoryLock->Acquire();
    int pageNo=memoryMap->Find();
    if(pageNo>= 0){
        pageUsed++;
    }
    memoryLock->Release();

    return pageNo;
}
void MemoryManager::FreePage(int physPageNum){
    if(pageUsed<= 0){
        return;
    }
    memoryLock->Acquire();
    memoryMap->Clear(physPageNum);
    pageUsed--;
    memoryLock->Release();

}
bool MemoryManager::PageIsAllocated(int physPageNum){
    bool state=false;
    memoryLock->Acquire();
    state=memoryMap->Test(physPageNum);

    memoryLock->Release();
    return state;
}
int MemoryManager::Alloc(int processNo,TranslationEntry *entry){
    if(pageUsed>= maxPages){
        return -1;
    }

    memoryLock->Acquire();

    int pageNo=memoryMap->Find();
    if(pageNo>= 0){
        entries[pageNo]=entry;
        processMap[pageNo]=processNo;
        pageUsed++;
    }
    memoryLock->Release();

    return pageNo;



}
int MemoryManager:: AllocByForce(int processNo,TranslationEntry *entry){
    memoryLock->Acquire();
    printf("picking page randomly in memorymanager.cc\n");

    //int pageNo= rand()%NumPhysPages;
    int pageNo= getLRUPageNo();
    int processId=processMap[pageNo];
    printf("process id is %d\n",processId);
    Thread* thread=(Thread*)processTable->Get(processId);
    printf("got thread with id with name %s\n",thread->getName());
    TranslationEntry* pte=entries[pageNo];
    thread->space->saveIntoSwapSpace(pte->virtualPage);
    entries[pageNo]=entry;
    processMap[pageNo]=processNo;

    memoryLock->Release();

    return pageNo;
}
int MemoryManager::getLRUPageNo(){
    printf("*****************\nSelecting page by LRU replacement\n*****************\n");
    int lowestPage=0;
    long int minTimeStamp=entries[0]->timestamp;
    for(int i=0;i<maxPages;i++){
        TranslationEntry* pte=entries[i];
        printf("P %d t %ld    ",i,pte->timestamp);
        if(pte->timestamp<minTimeStamp){
            minTimeStamp=pte->timestamp;
            lowestPage=i;
        }
    }
    printf("Page to replace %d with timestamp %ld\n",lowestPage,minTimeStamp);
    return lowestPage;


}

