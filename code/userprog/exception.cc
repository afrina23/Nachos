// exception.cc
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "syscall.h"
#include "processTable.h"
#include "thread.h"
#include "filemanager.h"

//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2.
//
// And don't forget to increment the pc before returning. (Or else you'll
// loop making the same system call forever!
//
//	"which" is the kind of exception.  The list of possible exceptions
//	are in machine.h.
//----------------------------------------------------------------------

void ExecSyscall();
void ExitSyscall();
void ReadSyscall();
void WriteSyscall();
void PageFaultCall();

extern ProcessTable* processTable;
extern SynchConsole* synchconsole;
extern MemoryManager* memoryManager;
void IncrementPC()
{
    int pc;

    pc = machine->ReadRegister(PCReg);
    machine->WriteRegister(PrevPCReg, pc);

    pc = machine->ReadRegister(NextPCReg);
    machine->WriteRegister(PCReg, pc);

    pc += 4;
    machine->WriteRegister(NextPCReg, pc);
}

void ForkForThread(void* arg){
    currentThread->space->InitRegisters();		// set the initial register values
    currentThread->space->RestoreState();		// load page table register


     machine->Run();			// jump to the user progam
     return;
}

void
ExceptionHandler(ExceptionType which)
{
  //  memoryManager=MemoryManager::getInstance();
    int type = machine->ReadRegister(2);

    if ((which == SyscallException) && (type == SC_Halt)) {
        DEBUG('a', "Shutdown, initiated by user program.\n");
        interrupt->Halt();
    }
    else if((which == SyscallException) && (type == SC_Exec)){
        ExecSyscall();
    }
    else if((which == SyscallException) && (type == SC_Exit)){
        ExitSyscall();
    }
    else if((which == SyscallException) && (type == SC_Read)){
        ReadSyscall();
    }
    else if((which == SyscallException) && (type == SC_Write)){
        WriteSyscall();
    }
    else if (which==PageFaultException){
        printf("PageFault exception has occured\n");
        PageFaultCall();

       // ExitSyscall();
    }
    else if(which==ReadOnlyException){
        printf("Read only exception has occured\n");
        ExitSyscall();
    }
    else if(which==BusErrorException){
        printf("Bus error exception has occured\n");
        ExitSyscall();
    }
    else if(which==AddressErrorException){
        printf("Address Error exception has occured\n");
        ExitSyscall();
    }
    else if(which==OverflowException){
        printf("OverFlow exception has occured\n");
        ExitSyscall();
    }
    else if(which==IllegalInstrException){
        printf("Illegal Instruction exception has occured\n");
        ExitSyscall();
    }
    else if(which==NumExceptionTypes){
        printf("NumExceptionTypes exception has occured\n");
        ExitSyscall();
    }
    else {
	printf("Unexpected user mode exception %d %d\n", which, type);
	//ASSERT(false);
	interrupt->Halt();
	ExitSyscall();
	//exit(0);
    }
    if(which==SyscallException){
         IncrementPC();
    }

}


void ExecSyscall(){
    //printf("Executiing System call EXEC\n");
    printf("in exception.cc/ExecSyscall\n");
    DEBUG('a', "Exec initiated by user program.\n");
    char * fileName=new char[MAX_FILE_SIZE];
    int offset=machine->ReadRegister(4);
    int i=0;
    while(true){
        bool state=machine->ReadMem(offset+i,1,(int*)&fileName[i]);
        if(!state){
          //  delete fileName;
           // fileName = new char[MAX_FILE_SIZE];
            machine->ReadMem(offset + i, 1, (int*)&fileName[i]);
            continue;

        }
        if(fileName[i]=='\0'){
            break;
        }
        i++;
        if(i>=MAX_FILE_SIZE){
            printf("File name is too big\n");
            machine->WriteRegister(2, 0);
            return;
        }
    }

    printf("File name is %s\n",fileName);
    OpenFile *executable = fileSystem->Open(fileName);
    AddrSpace *space;
     space = new AddrSpace(executable);


    if (executable == NULL) {
        printf("Unable to open file %s\n", fileName);
        machine->WriteRegister(2, -1);
        return;
    }
    else{
        Thread *thread= new Thread(fileName);

        int number=processTable->Alloc((void*)thread);
        thread->ID=number;
        bool condition=space->Initialize(executable,number);
        thread->space = space;



        if(number == -1){
            printf("Process can't be allocated\n");
             machine->WriteRegister(2, -1);
            return;

        }

        //delete executable;			// close file
        //delete fileName;
        machine->WriteRegister(2,number);

       // machine->Run();
        thread->Fork(ForkForThread,NULL);
       // currentThread->Yield();

    }


}
void ExitSyscall(){
    int status=machine->ReadRegister(4);
    printf("Exiting with status %d\n",status);

    int index=currentThread->ID;
    processTable->Release(index);
    printf("Process is released with status %d\n",status);
    delete currentThread->space;


    printf("Exiting from thread %s\n",currentThread->getName());
    printf("No of processes are %d\n",processTable->no_of_processes);
    if(processTable->no_of_processes==0){
        interrupt->Halt();
    }
    currentThread->Finish();
    //printf("No of processes are %d\n",processTable->no_of_processes);

}


void ReadSyscall(){
    printf("Reading system Call \n");
    int addr=machine->ReadRegister(4);
    int size=machine->ReadRegister(5);
    OpenFileId fileId=machine->ReadRegister(6);
    char* data= new char[size+1];
    int read_byte=synchconsole->Read(data,size);

    for(int i=0;i<size;i++){
        bool state= machine->WriteMem(addr+i,1,data[i]);
        if(!state){
            printf("Could not write the data to memory\n");
            if(machine->WriteMem(addr+i,1,data[i])){
               printf("writen successfully\n");
               }

            //machine->WriteRegister(2, -1);
            //return;
        }
        if(data[i]==NULL) break;
    }
    machine->WriteRegister(2,read_byte);


}
void WriteSyscall(){
    printf("Writing system Call \n");
    int addr=machine->ReadRegister(4);
    int size=machine->ReadRegister(5);
    OpenFileId fileId=machine->ReadRegister(6);
    char* data= new char[size];
    for(int i=0;i<size;i++){
        bool state=machine->ReadMem(addr+i,1,(int*)&data[i]);
        if(!state){
            printf("Could not read the data from memory\n");
            if(machine->ReadMem(addr+i,1,(int*)&data[i])){
                printf("read successfully\n");
            }

          //  machine->WriteRegister(2, -1);
            //return;
        }
    }
    synchconsole->Write(data,size);

}
void PageFaultCall(){
    unsigned int addr=machine->ReadRegister(BadVAddrReg);

    unsigned vpn=addr/PageSize;
    int ppn;
    printf("loading page %d with addr %d\n",vpn,addr);
    if(memoryManager->getFreePages()>0){
        //  ppn=memoryManager->AllocPage();

        ppn=memoryManager->Alloc(currentThread->ID,&(currentThread->space->pageTable[vpn]));
        printf("************Memory Page left %d   ************\n",memoryManager->getFreePages());

    }
    else{
        printf("*************There is no free pages. Page to evict %d\n",vpn);

        ppn=memoryManager->AllocByForce(currentThread->ID,&(currentThread->space->pageTable[vpn]));

    }
    currentThread->space->loadIntoFreePage(addr,ppn);
    stats->numPageFaults++;

}
