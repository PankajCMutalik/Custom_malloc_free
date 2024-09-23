#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define HEAP_SIZE 128 * 1024
#define BLOCK_SIZE 1024
#define TOTAL_BLOCKS (HEAP_SIZE / BLOCK_SIZE)

typedef struct {
    bool free_flag;
    size_t block_size;
} Chunk;

static uint8_t memory_heap[HEAP_SIZE];
static Chunk heap_chunks[TOTAL_BLOCKS];
static void* last_malloc = NULL;
static void* last_free = NULL;

void init_heap() {
    for (int i = 0; i < TOTAL_BLOCKS; i++) {
        heap_chunks[i].free_flag = true;
        heap_chunks[i].block_size = 1;
    }
}

void* my_malloc(size_t size) {
    size_t blocks_required = (size + BLOCK_SIZE - 1) / BLOCK_SIZE;
    size_t best_index = -1;
    size_t best_size = HEAP_SIZE + 1;

    for (int i = 0; i < TOTAL_BLOCKS; i++) {
        if (heap_chunks[i].free_flag) {
            size_t current_free_blocks = 0;
            int j = i;
            while (j < TOTAL_BLOCKS && heap_chunks[j].free_flag && current_free_blocks < blocks_required) {
                current_free_blocks++;
                j++;
            }
            if (current_free_blocks >= blocks_required && current_free_blocks < best_size) {
                best_size = current_free_blocks;
                best_index = i;
            }
        }
    }

    if (best_index == (size_t)-1) {
        return NULL;
    }

    for (size_t i = best_index; i < best_index + blocks_required; i++) {
        heap_chunks[i].free_flag = false;
        heap_chunks[i].block_size = blocks_required;
    }

    void* allocated_memory = &memory_heap[best_index * BLOCK_SIZE];
    if (allocated_memory == last_malloc) {
        return NULL;
    }
    last_malloc = allocated_memory;

    printf("Allocated %zuKB at address: %p\n", blocks_required, allocated_memory);
    return allocated_memory;
}

void my_free(void* ptr) {
    if (ptr == last_free) {
        printf("Error: Memory block already freed.\n");
        return;
    }

    size_t chunk_index = ((uint8_t*)ptr - memory_heap) / BLOCK_SIZE;
    size_t blocks_to_free = heap_chunks[chunk_index].block_size;
    for (size_t i = chunk_index; i < chunk_index + blocks_to_free; i++) {
        heap_chunks[i].free_flag = true;
        heap_chunks[i].block_size = 1;
    }

    last_free = ptr;
    printf("Freed memory at address: %p\n", ptr);
}

int main() {
    init_heap();

    void* alloc1 = my_malloc(sizeof(int) * 128);
    void* alloc2 = my_malloc(1000);
    void* alloc3 = my_malloc(128 * 8 * 1024);

    my_free(alloc1);
    my_free(alloc2);
    my_free(alloc3);

    return 0;
}

