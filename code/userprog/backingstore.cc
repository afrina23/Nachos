#include "copyright.h"
#include "filesys.h"
#include "system.h"
#include "translate.h"
#include "backingstore.h"
#include "openfile.h"
#include "string.h"


BackingStore::BackingStore(char* fname,int pages){
   // filename=fname;
    numPages=pages;
    filename=new char [100];
    //sprintf(filename, "swap_%s", fname);
    strcpy(filename,"Swap file for ");
    strcat(filename,fname);
    printf("swap file name %s\n",filename);
    int fileSize=numPages*PageSize;

    fileSystem->Create(filename, fileSize);

	swapfile = fileSystem->Open(filename);
    freemap= new bool[numPages];

    for(int i=0;i<numPages;i++){
        freemap[i]=false;
    }


}

void BackingStore::PageOut(TranslationEntry *pte){
    int vpn=pte->virtualPage;
    int ppn=pte->physicalPage;
    swapfile->WriteAt(&(machine->mainMemory[ppn*PageSize]),PageSize, vpn*PageSize);
    freemap[vpn]=true;

}
void BackingStore::PageIn(TranslationEntry *pte){
     int vpn=pte->virtualPage;
     int ppn=pte->physicalPage;
     swapfile->ReadAt(&(machine->mainMemory[ppn*PageSize]),PageSize,vpn*PageSize);


}
bool BackingStore::isPresent(int vpn){
    return freemap[vpn];

}
BackingStore::~BackingStore(){
    delete [] freemap;
    //fileSystem->Remove(filename);
    delete swapfile;
    delete filename;

}
