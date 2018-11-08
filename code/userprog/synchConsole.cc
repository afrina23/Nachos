#include "copyright.h"
#include "synchConsole.h"
#include "synch.h"
#include "system.h"


Semaphore *readAvail  = new Semaphore("read avail", 0);
Semaphore *writeDone  = new Semaphore("write done", 0);


void ReadAvail(void* arg) {
     readAvail->V();

}
void WriteDone(void* arg) {
    writeDone->V();
}


SynchConsole::SynchConsole(){
    console = new Console(NULL, NULL, ReadAvail, WriteDone, 0);
    consoleLock= new Lock("Console lock");

}
SynchConsole::~SynchConsole(){
    delete console;
}

char SynchConsole::synchGetChar(){
    char ch;
    readAvail->P();		// wait for character to arrive
	ch = console->GetChar();
    return ch;
}
void SynchConsole::synchPutChar(char ch){
    console->PutChar(ch);	// echo it!
	writeDone->P() ;        // wait for write to finish


}

int SynchConsole::Read (char* data,int size){
    consoleLock->Acquire();
    int i=0;
    for(i = 0; i < size; i++)
    {
        data[i] =synchGetChar();
        if(data[i]==NULL) break;
    }
    data[i] = '\0';

    consoleLock->Release();
    return i;
}




void SynchConsole::Write(char * data,int size){

    consoleLock->Acquire();

    for(int i = 0; i < size; i++)
    {
        synchPutChar(data[i]);
    }
    synchPutChar('\0');

    consoleLock->Release();

}

