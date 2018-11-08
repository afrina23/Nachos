#ifndef SYNC_CONSOLE_H
#define SYNC_CONSOLE_H

#include "copyright.h"
#include "utility.h"
#include "console.h"
#include "synch.h"
#include "system.h"

class SynchConsole{
public:

    SynchConsole();
    ~SynchConsole();
    char synchGetChar();
    void synchPutChar(char ch);
    void Write (char* data,int size);
    int Read (char* data,int size);


private:
    Console *console;
    Lock * consoleLock;

};




#endif
