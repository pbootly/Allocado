#include "allocado.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef TESTING
#include <assert.h>
void test_allocation(int size) {
  void *ptr1 = allocate(size);
  assert(ptr1 != NULL);
  printf("test_allocation_passed\n");

  free_block(ptr1);
  void *ptr2 = allocate(size);
  assert(ptr2 != NULL);
  printf("test_allocation passed again\n");
}

void test_many_allocation(int count) {
  for (int i = 0; i < count; i++) {
    allocate(128);
    visualize_pool();
  }
}

int main() {
  init_memory_pool(1024 * 1024);
  test_allocation(128);
}
#else

int main() {
  int count = 100;
  int values[] = {128, 512, 1024, 2048, 4096};
  int value_count = sizeof(values) / sizeof(values[1]);
  srand(time(NULL));
  init_memory_pool(1024 * 10248);
  void *allocated_pointers[count];
  int allocated_count = 0;

  for (int i = 0; i < count; i++) {
    int random_index = rand() % value_count;
    int random_value = values[random_index];
    if (rand() % 2 == 0 && allocated_count > 0) { // Random chance to free
      int random_free_index = rand() % allocated_count;
      void *ptr_to_free = allocated_pointers[random_free_index];
      printf("Freeing block at %p\n", ptr_to_free);
      free_block(ptr_to_free);

      allocated_pointers[random_free_index] =
          allocated_pointers[allocated_count - 1];
      allocated_count--;
    } else {
      void *ptr = allocate(random_value);
      if (ptr != NULL) {
        printf("Allocated %d bytes at %p\n", random_value, ptr);
        allocated_pointers[allocated_count++] = ptr;
      } else {
        printf("Failed to allocate %d bytes\n", random_value);
      }
    }
    visualize_pool();
  }
}
#endif