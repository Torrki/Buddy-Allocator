#include <stdio.h>
#include <stdlib.h>
#include "dynmem.h"

int main(int argc, char *argv[]){
  size_t sizeReq=1000, sizeEff=0;
  void* bufferMemory=malloc(sizeReq);
  if(bufferMemory){
    struct BuddyAllocator bAllo;
    printf("Buffer inizializzato: %p\n", bufferMemory);
    sizeEff=*(((size_t*)bufferMemory)-1);
    printf("Memoria richiesta: %ld\nMemoria effettiva: %ld\n",sizeReq,sizeEff);
    InitBuddy(bufferMemory,sizeEff,&bAllo,6);
    void* addr1=myMalloc(&bAllo,10);
    void* addr7=myMalloc(&bAllo,20);
    void* addr2=myMalloc(&bAllo,30);
    void* addr4=myMalloc(&bAllo,20);
    void* addr6=myMalloc(&bAllo,250);
    void* addr5=myMalloc(&bAllo,10);
    void* addr3=myMalloc(&bAllo,80);
    myFree(&bAllo,addr1);
    myFree(&bAllo,addr2);
    myFree(&bAllo,addr3);
    myFree(&bAllo,addr4);
    myFree(&bAllo,addr5);
    myFree(&bAllo,addr6);
    myFree(&bAllo,addr7);
    
    FreeBuddy(&bAllo);
    free(bufferMemory);
  }
  return 0;
}
