#ifndef MEMORY_ALLOCATOR_H
#define MEMORY_ALLOCATOR_H

#include <stddef.h>

struct BlockHeader {
    size_t size; // Block size
    int is_free; // Is block free (1) or not (0)
    struct BlockHeader* next; // Pointer to next block in pool
};

void init_memory_pool(int pool_size);
void* allocate(size_t size);
void free_block(void* ptr);
void free_memory_pool(void);
void coalesce(void);
void visualize_pool(void);

// Request more memory from the system
void* request_more_memory(size_t size);
void split_block_if_needed(struct BlockHeader* current, size_t size);


#endif
