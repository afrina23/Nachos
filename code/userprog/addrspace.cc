// addrspace.cc
//	Routines to manage address spaces (executing user programs).
//
//	In order to run a user program, you must:
//
//	1. link with the -N -T 0 option
//	2. run coff2noff to convert the object file to Nachos format
//		(Nachos object code format is essentially just a simpler
//		version of the UNIX executable object code format)
//	3. load the NOFF file into the Nachos file system
//		(if you haven't implemented the file system yet, you
//		don't need to do this last step)
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "addrspace.h"
#include "utility.h"
#include "filemanager.h"
#include <algorithm>

//----------------------------------------------------------------------
// SwapHeader
// 	Do little endian to big endian conversion on the bytes in the
//	object file header, in case the file was generated on a little
//	endian machine, and we're now running on a big endian machine.
//----------------------------------------------------------------------

static void
SwapHeader (NoffHeader *noffH)
{
	noffH->noffMagic = WordToHost(noffH->noffMagic);
	noffH->code.size = WordToHost(noffH->code.size);
	noffH->code.virtualAddr = WordToHost(noffH->code.virtualAddr);
	noffH->code.inFileAddr = WordToHost(noffH->code.inFileAddr);
	noffH->initData.size = WordToHost(noffH->initData.size);
	noffH->initData.virtualAddr = WordToHost(noffH->initData.virtualAddr);
	noffH->initData.inFileAddr = WordToHost(noffH->initData.inFileAddr);
	noffH->uninitData.size = WordToHost(noffH->uninitData.size);
	noffH->uninitData.virtualAddr = WordToHost(noffH->uninitData.virtualAddr);
	noffH->uninitData.inFileAddr = WordToHost(noffH->uninitData.inFileAddr);
}

extern MemoryManager* memoryManager;
//----------------------------------------------------------------------
// AddrSpace::AddrSpace
// 	Create an address space to run a user program.
//	Load the program from a file "executable", and set everything
//	up so that we can start executing user instructions.
//
//	Assumes that the object code file is in NOFF format.
//
//	First, set up the translation from program memory to physical
//	memory.  For now, this is really simple (1:1), since we are
//	only uniprogramming, and we have a single unsegmented page table
//
//	"executable" is the file containing the object code to load into memory
//----------------------------------------------------------------------

AddrSpace::AddrSpace(OpenFile *executable)
{

}
bool AddrSpace::Initialize(OpenFile *executable,int fname){
    NoffHeader noffH;
    filename=new char[50];
    sprintf(filename, "swap %d",fname);
    unsigned int i, size;
    this->executable=executable;
    //memoryManager= MemoryManager::getInstance();
    executable->ReadAt((char *)&noffH, sizeof(noffH), 0);
    if ((noffH.noffMagic != NOFFMAGIC) &&
		(WordToHost(noffH.noffMagic) == NOFFMAGIC))
    	SwapHeader(&noffH);
    ASSERT(noffH.noffMagic == NOFFMAGIC);
    this->noffHead=noffH;
// how big is address space?
    size = noffH.code.size + noffH.initData.size + noffH.uninitData.size
			+ UserStackSize;	// we need to increase the size
						// to leave room for the stack
    numPages = divRoundUp(size, PageSize);
    size = numPages * PageSize;
    swapspace= new BackingStore((char*)filename,numPages);
    //if(numPages > memoryManager->getFreePages()){
      //  return false;
    //}
   // ASSERT(numPages <= memoryManager->getFreePages())
   // ASSERT(numPages <= NumPhysPages);		// check we're not trying
						// to run anything too big --
						// at least until we have
						// virtual memory

    DEBUG('a', "Initializing address space, num pages %d, size %d\n",
					numPages, size);
// first, set up the translation
    pageTable = new TranslationEntry[numPages];
    for (i = 0; i < numPages; i++) {
	pageTable[i].virtualPage = i;	// for now, virtual page # = phys page #
	//pageTable[i].physicalPage = memoryManager->AllocPage();
	pageTable[i].physicalPage =-1;
	//pageTable[i].valid = true;
	pageTable[i].valid = false;
	pageTable[i].use = false;
	pageTable[i].timestamp=0;
	pageTable[i].dirty = false;
	pageTable[i].readOnly = false;  // if the code segment was entirely on
					// a separate page, we could set its
					// pages to be read-only
    }
// zero out the entire address space, to zero the unitialized data segment
// and the stack segment
   // bzero(machine->mainMemory, size);

   /*for(int j=0;j<numPages;j++){
       bzero( &(machine->mainMemory[pageTable[j].physicalPage*PageSize]),PageSize);
   }

// then, copy in the code and data segments into memory
    int real_size=noffH.code.size+noffH.initData.size;
    int page_num=divRoundUp(real_size,PageSize);
    int start_addr=noffH.code.virtualAddr;
    int Page_no=0;
    int page_read=0;
    int base_addr=noffH.code.inFileAddr;
    if(real_size>0){
        DEBUG('a', "Initializing code segment, at 0x%x, size %d\n",
			noffH.code.virtualAddr, noffH.code.size);
         DEBUG('a', "Initializing data segment, at 0x%x, size %d\n",
			noffH.initData.virtualAddr, noffH.initData.size);

        if(start_addr%PageSize != 0){
            int to_read=PageSize-(start_addr%PageSize);
            executable->ReadAt(&(machine->mainMemory[pageTable[Page_no].physicalPage*PageSize]),
                to_read, base_addr);
                page_read+=to_read;
                Page_no++;

        }
        while((page_read+PageSize)<=real_size){
            executable->ReadAt(&(machine->mainMemory[pageTable[Page_no].physicalPage*PageSize]),
                PageSize, base_addr+(Page_no*PageSize));
            Page_no++;
            page_read+=PageSize;

        }
        if(page_read<real_size){
            int to_read=real_size-page_read;
            executable->ReadAt(&(machine->mainMemory[pageTable[Page_no].physicalPage*PageSize]),
                to_read,base_addr+page_read);

        }
    }

   /* int code_page=divRoundUp(noffH.code.size,PageSize);
    if (noffH.code.size > 0) {
        DEBUG('a', "Initializing code segment, at 0x%x, size %d\n",
			noffH.code.virtualAddr, noffH.code.size);
    //    executable->ReadAt(&(machine->mainMemory[noffH.code.virtualAddr]),
		//	noffH.code.size, noffH.code.inFileAddr);
		for(int i=0;i<code_page;i++){
                executable->ReadAt(&(machine->mainMemory[pageTable[i].physicalPage*PageSize]),
                PageSize, noffH.code.inFileAddr+(i*PageSize));

		}

    }
    int data_page=divRoundUp(noffH.initData.size,PageSize);
    if (noffH.initData.size > 0) {
        DEBUG('a', "Initializing data segment, at 0x%x, size %d\n",
			noffH.initData.virtualAddr, noffH.initData.size);
      //  executable->ReadAt(&(machine->mainMemory[noffH.initData.virtualAddr]),
		//	noffH.initData.size, noffH.initData.inFileAddr);
		for(int i=code_page;i<data_page;i++){
             executable->ReadAt(&(machine->mainMemory[pageTable[i].physicalPage*PageSize]),
                PageSize, noffH.initData.inFileAddr+(i*PageSize));

		}
    }*/
    return true;

}

//----------------------------------------------------------------------
// AddrSpace::~AddrSpace
// 	Dealloate an address space.  Nothing for now!
//----------------------------------------------------------------------

AddrSpace::~AddrSpace()
{
    ReleasePages();
    delete executable;
    delete pageTable;
    delete swapspace;
   //delete noffHead;
}

//----------------------------------------------------------------------
// AddrSpace::InitRegisters
// 	Set the initial values for the user-level register set.
//
// 	We write these directly into the "machine" registers, so
//	that we can immediately jump to user code.  Note that these
//	will be saved/restored into the currentThread->userRegisters
//	when this thread is context switched out.
//----------------------------------------------------------------------

void
AddrSpace::InitRegisters()
{
    int i;

    for (i = 0; i < NumTotalRegs; i++)
	machine->WriteRegister(i, 0);

    // Initial program counter -- must be location of "Start"
    machine->WriteRegister(PCReg, 0);

    // Need to also tell MIPS where next instruction is, because
    // of branch delay possibility
    machine->WriteRegister(NextPCReg, 4);

   // Set the stack register to the end of the address space, where we
   // allocated the stack; but subtract off a bit, to make sure we don't
   // accidentally reference off the end!
    machine->WriteRegister(StackReg, numPages * PageSize - 16);
    DEBUG('a', "Initializing stack register to %d\n", numPages * PageSize - 16);
}

//----------------------------------------------------------------------
// AddrSpace::SaveState
// 	On a context switch, save any machine state, specific
//	to this address space, that needs saving.
//
//	For now, nothing!
//----------------------------------------------------------------------

void AddrSpace::SaveState()
{}

//----------------------------------------------------------------------
// AddrSpace::RestoreState
// 	On a context switch, restore the machine state so that
//	this address space can run.
//
//      For now, tell the machine where to find the page table.
//----------------------------------------------------------------------

void AddrSpace::RestoreState()
{
    machine->pageTable = pageTable;
    machine->pageTableSize = numPages;
}
void AddrSpace::ReleasePages(){
    for (int i = 0; i < numPages; i++)
	{
	    if(pageTable[i].valid){
            memoryManager->FreePage(pageTable[i].physicalPage);
	    }

	}
}

int AddrSpace::loadIntoFreePage(unsigned addr, int physicalPageNo){


        int vpn=addr/PageSize;
		         pageTable[vpn].physicalPage =physicalPageNo;
        pageTable[vpn].valid = true;

        if(isSwapPageExists(vpn)){
            printf("taking page from swap space\n");
            loadFromSwapSpace(vpn);

        }
        else{
            printf("taking page from disk space\n");
            int base_code_vaddr=noffHead.code.virtualAddr;
            int code_size=noffHead.code.size;
            int base_code_faddr=noffHead.code.inFileAddr;
            int full_size=code_size+noffHead.initData.size;
            bzero(&(machine->mainMemory[physicalPageNo*PageSize]),PageSize);
          //  printf("base code %d     size %d    base idata %d   size %d  base udata %d size %d\n\n",base_code_faddr,code_size
            //   ,base_idata_faddr,idata_size,noffHead.uninitData.inFileAddr,noffHead.uninitData.size);

            int to_read=std::min(PageSize,full_size);
            executable->ReadAt(&(machine->mainMemory[physicalPageNo*PageSize]),
                to_read,base_code_faddr+vpn*PageSize);
            printf("have to read %d Pagesize %d to mem %d\n",base_code_faddr+vpn*PageSize,PageSize,
                &(machine->mainMemory[physicalPageNo*PageSize]));

        }




        printf("Page is loaded %d numpage %d\n",vpn,numPages);
		return 0;
}
void AddrSpace::saveIntoSwapSpace(int vpn){
    TranslationEntry pte=pageTable[vpn];
    swapspace->PageOut(&pte);
    pageTable[vpn].dirty = false;
	pageTable[vpn].valid = false;


}
void AddrSpace:: loadFromSwapSpace(int vpn){
    TranslationEntry pte=pageTable[vpn];
    swapspace->PageIn(&pte);

}
bool AddrSpace::isSwapPageExists(int vpn){

     return swapspace->isPresent(vpn);


}
