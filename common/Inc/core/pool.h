#ifndef INC_POOL_H_
#define INC_POOL_H_

#include <stdint.h>
#include <stddef.h>

/* Configuration parameters*/

#define POOL_MEMORY_SIZE        20 // number of memory pools
#define POOL_BLOCK_SIZE         32 // Size of each block in bytes
#define POOL_ALIGNAMENT         4  // Align memory blocks to 4 bytes

/* Memory pool data structure */

typedef struct MemBlock{
    struct MemBlock *next;
}MemBlock_t;

typedef struct
{
    uint8_t pool[POOL_MEMORY_SIZE][POOL_BLOCK_SIZE]; // Pre-allocated memory
    MemBlock_t *freelist; // Linked list of free block
}MemPool_t;

void pool_Init(void);
void *pool_Allocate(void);
void pool_free(void *ptr);

uint32_t pool_GetFreeBlockCount(void);
uint8_t pool_IsFromPool(void *ptr);

#endif /* INC_POOL_H_ */