#ifndef BACKINGSTORE_H
#define BACKINGSTORE_H

#include "copyright.h"
#include "filesys.h"

#include "translate.h"
#include "addrspace.h"
#include "openfile.h"
class BackingStore{
public:
    BackingStore(char* fname,int pages);
    ~BackingStore();
    void PageOut(TranslationEntry *pte);
    void PageIn(TranslationEntry *pte);
    bool isPresent(int vpn);

private:
    OpenFile * swapfile;
    char* filename;
    int numPages;
    bool *freemap;


};














#endif // BACKINGSTORE_H
