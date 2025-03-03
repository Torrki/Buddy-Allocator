#include <stdlib.h>
#include <stdint.h>

#pragma pack(1)
struct BuddyAllocator{
  size_t sizeBuffer;
  uint8_t nLivelli;
  uint64_t albero;
  void* startAddress;
};

int InitBuddy(void* buffer, size_t dim, struct BuddyAllocator *allocator, uint8_t liv);
int FreeBuddy(struct BuddyAllocator *allocator);
void* myMalloc(struct BuddyAllocator *allocator, size_t req);
void myFree(struct BuddyAllocator *allocator, void* address);
