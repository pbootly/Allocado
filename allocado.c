#include "allocado.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>

void *memory_pool;
struct BlockHeader *free_list;

void init_memory_pool(int pool_size) {
  // memory_pool = malloc(MEMORY_POOL_SIZE);
  memory_pool = mmap(NULL, pool_size, PROT_READ | PROT_WRITE,
                     MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
  if (memory_pool == MAP_FAILED) {
    perror("mmap failed");
    exit(1);
  }

  // Init the first block in pool
  free_list = (struct BlockHeader *)memory_pool;
  free_list->size = pool_size - sizeof(struct BlockHeader);
  free_list->is_free = 1;
  free_list->next = NULL;
}

void *allocate(size_t size) {
  struct BlockHeader *current = free_list;
  while (current != NULL) {
    if (current->is_free && current->size >= size) {
      if (current->size > size + sizeof(struct BlockHeader)) {
        split_block_if_needed(current, size);
      }
      current->is_free = 0; // Mark the block as used
      return (void *)((char *)current + sizeof(struct BlockHeader));
    }
    current = current->next;
  }

  return request_more_memory(size);
}

void *request_more_memory(size_t size) {
  size_t block_size = size + sizeof(struct BlockHeader);
  void *new_block = mmap(NULL, block_size, PROT_READ | PROT_WRITE,
                         MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
  if (new_block == MAP_FAILED) {
    perror("mmap failed for more memory");
    return NULL;
  }

  // Init new block
  struct BlockHeader *new_header = (struct BlockHeader *)new_block;
  new_header->size = block_size;
  new_header->is_free = 0; // Initially allocated, so not free
  new_header->next = NULL;

  struct BlockHeader *current = free_list;
  while (current->next != NULL) {
    current = current->next;
  }
  current->next = new_header;
  return (void *)((char *)new_header + sizeof(struct BlockHeader));
}

void split_block_if_needed(struct BlockHeader *current, size_t size) {
  if (current->size > size + sizeof(struct BlockHeader)) {
    struct BlockHeader *next_block =
        (struct BlockHeader *)((char *)current + size +
                               sizeof(struct BlockHeader));

    next_block->size =
        current->size - size -
        sizeof(struct BlockHeader); // Remaining space after split
    next_block->is_free = 1;
    next_block->next = current->next;

    // Update the current block
    current->size = size;
    current->next = next_block;
  }
}

struct BlockHeader *next_block(struct BlockHeader *current) {
  return current->next;
}

void free_block(void *ptr) {
  struct BlockHeader *block =
      (struct BlockHeader *)((char *)ptr - sizeof(struct BlockHeader));
  block->is_free = 1;
  coalesce();
}

void coalesce() {
  struct BlockHeader *current = free_list;
  while (current != NULL && current->next != NULL) {
    if (current->is_free && current->next->is_free) {
      current->size += sizeof(struct BlockHeader) + current->next->size;
      current->next = current->next->next;
    } else {
      current = current->next;
    }
  }
}

void visualize_pool() {
  struct BlockHeader *current = free_list;
  printf("Memory Pool Visualization:\n");
  while (current != NULL) {
    printf("Block at %p: Size = %zu, %s\n", (void *)current, current->size,
           current->is_free ? "Free" : "Allocated");
    current = current->next;
  }
  printf("\n");
}