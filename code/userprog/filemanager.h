#ifndef FILEMANAGER_H
#define FILEMANAGER_H



#include "copyright.h"
#include "synch.h"
#include "bitmap.h"
#include "translate.h"

class MemoryManager{
public:
   static  MemoryManager* getInstance();
   ~MemoryManager();
   int AllocPage();
   void FreePage(int physPageNum);
   bool PageIsAllocated(int physPageNum);
   int getFreePages(){return (maxPages-pageUsed);}
   int Alloc(int processNo,TranslationEntry *entry);
   int AllocByForce(int processNo,TranslationEntry *entry);
   int  getLRUPageNo();
private:
    Lock* memoryLock;
    BitMap *memoryMap;
    int pageUsed;
    int maxPages;
    int *processMap;
    TranslationEntry **entries;

    MemoryManager(int numPages);
    static MemoryManager* manager;
    static bool createInstance;

};





#endif
