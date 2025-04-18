#include <stdio.h>
#include "dynmem.h"

int InitBuddy(void* buffer, size_t dim, struct BuddyAllocator *allocator, uint8_t liv){
  if(buffer == NULL || dim < 1 || allocator == NULL || liv < 1) return 1;
  allocator->sizeBuffer=(dim >> (liv-1)) << (liv-1);
  //printf("Dimensione albero: %ld\n",allocator->sizeBuffer);
  allocator->nLivelli=liv;
  allocator->albero=0x0;
  allocator->startAddress=buffer;
  return 0;
}

int FreeBuddy(struct BuddyAllocator *allocator){
  if(allocator==NULL) return 1;
  allocator->startAddress=NULL;
  allocator->sizeBuffer=0;
  allocator->albero=0x0;
  allocator->nLivelli=0;
  return 0;
}

void* myMalloc(struct BuddyAllocator *allocator, size_t req){
  if(allocator == NULL || req == 0) return NULL;
  
  size_t dimBloccoEle=allocator->sizeBuffer >> (allocator->nLivelli-1), tmpDimensione=allocator->sizeBuffer;
  //size_t dimMemoriaAlbero=dimBloccoEle << (allocator->nLivelli-1);
  uint8_t livello=0;
  uint64_t maskBit=1UL<<( sizeof(uint64_t)*8 - 1);
  uint32_t nodoScelto=0;
  
  //Divido fino a quando la dimensione trovata non è uguale alla dimensione minima allocabile
  while(tmpDimensione > dimBloccoEle && (tmpDimensione >> 1) > req){
    ++livello;
    maskBit >>= (1<<(livello-1));
    tmpDimensione >>= 1;
  }
  
  //printf("Dimensione da allocare %ld\n", tmpDimensione);
  
  //Trovo il primo nodo del livello disponibile e lo occupo, se esiste
  while(nodoScelto < (1 << livello) && allocator->albero & maskBit){
    maskBit >>= 1;
    ++nodoScelto;
  }
  //printf("nodo: %u\n", nodoScelto);
  if(nodoScelto == (1 << livello)){
    //fprintf(stderr,"Non è stato possibile allocare questo spazio!\n");
    return NULL;
  }else{
    allocator->albero |= maskBit;
  
    //Modifica dell'albero per segnalare i blocchi allocati nei livelli superiori e inferiori
    uint8_t tmpLivello=livello;
    uint32_t tmpnodoScelto=nodoScelto, nFigli=2;
    uint64_t tmpMaskBit=maskBit;
    while(tmpLivello < (allocator->nLivelli)-1){
      tmpMaskBit >>= (1 << tmpLivello)-tmpnodoScelto+(tmpnodoScelto << 1);
      tmpMaskBit |= tmpMaskBit >> (nFigli >> 1);
      allocator->albero |= tmpMaskBit;
      nFigli <<= 1;
      tmpnodoScelto <<= 1;
      ++tmpLivello;
    }
    tmpLivello=livello;
    tmpnodoScelto=nodoScelto;
    while((maskBit & ( 1UL<<( sizeof(uint64_t)*8 - 1) )) == 0){
      maskBit <<= tmpnodoScelto + (1UL<<(--tmpLivello)) - (tmpnodoScelto >> 1);
      tmpnodoScelto >>= 1;
      allocator->albero |= maskBit;
      //printf("%lb\n%lb\n\n",allocator->albero,1UL<<( sizeof(uint64_t)*8 - 1));
    }
    //printf("albero allocazione: %064lb\n", allocator->albero);
    //printf("dimensione: %lu\n\n", tmpDimensione);
    //printf("Indirizzo allocato: %p\n", allocator->startAddress+(nodoScelto*tmpDimensione)+sizeof(uint8_t));
    *((uint8_t*)(allocator->startAddress+(nodoScelto*tmpDimensione)))=livello;
    //printf("livello %hhu\n", livello);
    return allocator->startAddress+(nodoScelto*tmpDimensione)+sizeof(uint8_t);
  }
}

void myFree(struct BuddyAllocator *allocator, void* address){
  if(allocator != NULL && address != NULL){
    uint8_t livello=*(((uint8_t*)(address))-1), merge=1;
    //printf("Deallocazione: %hhu\n", livello);
    
    //individuare l'indice del nodo del livello e azzerare tutti i bit di segnalazione nei livelli successivi, ed eventualmente fare il merge dei livelli precedenti
    //size_t dimMemoriaAlbero=(allocator->sizeBuffer >> (allocator->nLivelli-1)) << (allocator->nLivelli-1);
    uint32_t iNodo=0, nodiProcessati=0;
    uint64_t maskBit=0x0;
    for(int i=allocator->nLivelli-1; i>=0 && merge; --i){
      iNodo=((address-sizeof(uint8_t))-allocator->startAddress)/(allocator->sizeBuffer>>i);
      //printf("iLivello: %u\n", iNodo);
      maskBit = 1UL << ((1<<i)-iNodo);
      
      //Modifica albero
      if(i > livello){
        //printf("Numero blocchi da deallocare: %u\n", 1<<(i-livello));
        maskBit = ( (1UL << ( 1 << (i-livello) ))-1UL ) << ( (1<<i)-(1<<(i-livello))+nodiProcessati-iNodo+1 );
        allocator->albero &= ~maskBit;
      }else{
        maskBit = 1UL << ((1<<i)-iNodo+nodiProcessati);
        allocator->albero &= ~maskBit;
        if(iNodo % 2){  //Se nodo pari
          if(i > 0 && allocator->albero & (maskBit << 1)) merge=0;
        }else{
          if(allocator->albero & (maskBit >> 1)) merge=0;
        }
      }
      
      nodiProcessati += 1<<i;
      //printf("mask: %064lb\nalbero allocazione: %064lb\n\n", maskBit, allocator->albero);
    }
    //printf("iNodo: %u\n", iNodo);
  }
}
