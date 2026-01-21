#include "core/pool.h"
#include <string.h>

static MemPool_t memPool;

void pool_Init(void)
{
    memPool.freelist = NULL;

    for(uint32_t i = 0; i < POOL_MEMORY_SIZE; i++)
    {
        MemBlock_t *block = (MemBlock_t*)&memPool.pool[i][0];
        block->next = memPool.freelist;
        memPool.freelist = block;
    }
}

void *pool_Allocate(void)
{

}

void pool_free(void *ptr)
{

}

uint32_t pool_GetFreeBlockCount(void)
{

}

uint8_t pool_IsFromPool(void *ptr)
{
    if(ptr == NULL)
    {
        return 0;
    }

    uint8_t *bytePtr   = (uint8_t*)ptr;
    uint8_t *poolStart = (uint8_t*)memPool.pool[0][0];
    uint8_t *poolEnd   = (uint8_t*)memPool.pool[POOL_MEMORY_SIZE][POOL_BLOCK_SIZE];

    if(bytePtr >= poolStart && bytePtr <= poolEnd)
    {
        return 1;
    }
    return 0;
}
