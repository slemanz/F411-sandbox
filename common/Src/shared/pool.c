#include "shared/pool.h"
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
    if(memPool.freelist == NULL)
    {
        return NULL;
    }

    MemBlock_t *block = memPool.freelist;
    memPool.freelist = block->next;

    memset(block, 0, POOL_BLOCK_SIZE);

    return (void*)block;
}

void pool_free(void *ptr)
{
    if(ptr == NULL)
    {
        return;
    }

    if(!pool_IsFromPool(ptr))
    {
        return;
    }

    MemBlock_t *block = (MemBlock_t*)ptr;
    block->next = memPool.freelist;
    memPool.freelist = block;
}

uint32_t pool_GetFreeBlockCount(void)
{
    uint32_t count = 0;
    MemBlock_t *current = memPool.freelist;
    
    while (current != NULL)
    {
        count++;
        current = current->next;
    }
    
    return count;
}

uint8_t pool_IsFromPool(void *ptr)
{
    if(ptr == NULL)
    {
        return 0;
    }

    uint8_t *bytePtr   = (uint8_t*)ptr;
    uint8_t *poolStart = (uint8_t*)&memPool.pool[0][0];
    uint8_t *poolEnd   = (uint8_t*)&memPool.pool[POOL_MEMORY_SIZE][POOL_BLOCK_SIZE];

    if(bytePtr >= poolStart && bytePtr <= poolEnd)
    {
        return 1;
    }
    return 0;
}
