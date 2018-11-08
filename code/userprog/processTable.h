#ifndef PROCESSTABLE_H
#define PROCESSTABLE_H



#include "copyright.h"
#include "synch.h"


class ProcessTable{
public:
   static  ProcessTable* getInstance(int Size);
   ~ProcessTable();
    int Alloc(void * process);
    void *Get(int index);
    void Release(int index);
    int no_of_processes;
    int max_processes;
    void **processArray;
private:
    Lock* processTableLock;
    ProcessTable(int size);
    static ProcessTable* processTable;
    static bool createInstance;

};





#endif
