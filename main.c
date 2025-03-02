#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]){
  size_t sizeReq=1000, sizeEff=0;
  void* bufferMemory=malloc(sizeReq);
  if(bufferMemory){
    printf("Buffer inizializzato: %p\n", bufferMemory);
    sizeEff=*(((size_t*)bufferMemory)-1);
    printf("Memoria richiesta: %ld\nMemoria effettiva: %ld\n",sizeReq,sizeEff);
    
    //Buddy allocator con 5 livelli dell'albero
    int nLivelli=6;
    size_t dimBloccoEle=sizeEff >> (nLivelli-1);
    size_t dimMemoriaAlbero=(dimBloccoEle << (nLivelli-1));
    unsigned long alberoDivisioni=0x0;
    printf("Con una divisione in %d livelli lo scarto è di %ld (%ld-%ld)\nBlocco elementare di %ld\n",nLivelli,
                                                                                                      sizeEff-(dimBloccoEle << (nLivelli-1)),sizeEff,dimMemoriaAlbero,dimBloccoEle);
    
    //Richiesta di una porzione
    size_t Requests[]={10,30,20,20,80,800,250};
    for(int i=0; i<7; ++i){
      size_t clientRequest=Requests[i];
      size_t tmpDimensione=dimMemoriaAlbero;
      unsigned livello=0;
      unsigned long maskBit=1UL<<( sizeof(long)*8 - 1);
      printf("albero allocazione: %064lb\n\n", alberoDivisioni);
      
      //Divido fino a quando la dimensione trovata non è uguale alla dimensione minima allocabile
      while(tmpDimensione > dimBloccoEle && (tmpDimensione >> 1) > clientRequest){
        ++livello;
        maskBit >>= (1<<(livello-1));
        tmpDimensione >>= 1;
      }
      
      printf("Dimensione da allocare %ld\n", tmpDimensione);
      
      //Trovo il primo nodo del livello disponibile e lo occupo, se esiste
      unsigned nodoScelto=0;
      while(nodoScelto < (1 << livello) && alberoDivisioni & maskBit){
        maskBit >>= 1;
        ++nodoScelto;
      }
      if(nodoScelto == (1 << livello)){
        fprintf(stderr,"Non è stato possibile allocare questo spazio!\n");
      }else{
        alberoDivisioni |= maskBit;
      
        //Modifica dell'albero per segnalare i blocchi allocati nei livelli superiori e inferiori
        unsigned tmpLivello=livello, tmpnodoScelto=nodoScelto;
        unsigned nFigli=2;
        unsigned long tmpMaskBit=maskBit;
        while(tmpLivello < nLivelli-1){
          tmpMaskBit >>= (1 << tmpLivello)-tmpnodoScelto+(tmpnodoScelto << 1);
          tmpMaskBit |= tmpMaskBit >> (nFigli >> 1);
          alberoDivisioni |= tmpMaskBit;
          nFigli <<= 1;
          tmpnodoScelto <<= 1;
          ++tmpLivello;
        }
        tmpnodoScelto=nodoScelto;
        while((maskBit & ( 1UL<<( sizeof(long)*8 - 1) )) == 0){
          maskBit <<= tmpnodoScelto + (1<<(--livello)) - (tmpnodoScelto >> 1);
          tmpnodoScelto >>= 1;
          alberoDivisioni |= maskBit;
        }        
        printf("Indirizzo allocato: %p\n", bufferMemory+(nodoScelto*tmpDimensione));
      }
    }
    printf("albero allocazione: %064lb\n", alberoDivisioni);
    free(bufferMemory);
  }
  return 0;
}
