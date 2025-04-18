#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
// Include your Headers below
#include <unistd.h>  // For sbrk
#include <string.h>  // For memset and memcpy
#include <stdio.h>   // For NULL
// #include <sys/mman.h> // For mmap
#include <stddef.h>

// You are not allowed to use the function malloc and calloc directly .
struct headerstruct {
    size_t size;          // Size of the memory block
    int free;             // Free flag (1 = free, 0 = allocated)
    struct headerstruct* next;  // Pointer to the next block in the linked list
    int mmapalloc;
};

#define META_SIZE sizeof(struct headerstruct)  // Size of the metadata block
#define MMAP_THRESHOLD 128 * 1024  // Threshold for using mmap for large allocations (128 KB)

struct headerstruct* base = NULL;  // Base (head) of the linked list of blocks

// Find a free block of sufficient size in the linked list
struct headerstruct* find_free_block(struct headerstruct** last, size_t size) {
    struct headerstruct* current = base;
    while (current && !(current->free && current->size >= size)) {
        *last = current;
        current = current->next;
    }
    return current;
}

// Request more space from the system using sbrk or mmap
struct headerstruct* request_space(struct headerstruct* last, size_t size) {
    struct headerstruct* block;
    if (size >= MMAP_THRESHOLD) {
        // Use mmap for large allocations
        block = (struct headerstruct*)mmap(NULL, size + META_SIZE, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
        if (block == MAP_FAILED) {
            return NULL;
        }
        block->size = size;
        block->free = 0;
        block->next = NULL;
        block->mmapalloc=1;
        if (last) {
            last->next = block;
        }
        return block;
    }

    block = (struct headerstruct*)sbrk(0);  // Get the current program break
    void* request = sbrk(size + META_SIZE);  // Move the program break by the required size
    if (request == (void*)-1) {
        return NULL;  // sbrk failed
    }

    // Initialize the new block
    block->size = size;
    block->free = 0;
    block->next = NULL;
     block->mmapalloc=0;

    if (last) {
        last->next = block;  // Link to the last block in the list
    }
    return block;
}

void split_block(struct headerstruct* block, size_t size) {
    // If the block size is significantly larger than the requested size, split it
    if (block->size >= size + META_SIZE + 4) {  // Ensure enough space for a new block
        struct headerstruct* new_block = (struct headerstruct*)((char*)block + META_SIZE + size);
        new_block->size = block->size - size - META_SIZE;
        new_block->free = 1;  // New block is free
        new_block->next = block->next;  // Point to the next block in the list
        block->size = size;  // Adjust the size of the current block
        block->next = new_block;  // Link the new block to the current block
        new_block->mmapalloc=0;
    }
    block->free = 0;  // Mark the current block as allocated
}

void* my_malloc(size_t size) {
    struct headerstruct* block;

    if (size <= 0) {
        return NULL;  // Invalid size
    }

    // If there are no existing blocks (first allocation)
    if (!base) {
        block = request_space(NULL, size);
        if (!block) {
            return NULL;  // Allocation failed
        }
        base = block;  // Set the base to the first block
    } else {
        struct headerstruct* last = base;
        block = find_free_block(&last, size);  // Find a free block

        if (block) {
            // If a suitable free block is found, split it if necessary
            if (block->size > size + META_SIZE + 4) {
                split_block(block, size);
            }
            block->free = 0;  // Mark block as used
        } else {
            block = request_space(last, size);  // No suitable block found, request new space
            if (!block) {
                return NULL;  // Allocation failed
            }
        }
    }

    return (block + 1);  // Return the pointer after the meta block
}

// Custom calloc implementation
void* my_calloc(size_t nelem, size_t size) {
    size_t total_size = nelem * size;
    void* ptr = my_malloc(total_size);  // Allocate memory
    if (ptr) {
        memset(ptr, 0, total_size);  // Initialize allocated memory to zero
    }
    return ptr;
}

// Custom free function with coalescing of adjacent free blocks
void my_free(void* ptr) {
    if (!ptr) {
        return;  // Nothing to free
    }
    

    // Get the meta block corresponding to the memory block
    struct headerstruct* block = (struct headerstruct*)ptr - 1;
    block->free = 1;  // Mark block as free
    if (block->size >= MMAP_THRESHOLD && (block->mmapalloc==1)) {
            struct headerstruct* current = base;
            while (current && current->next != block) {
                current = current->next;  // Find the block just before the current block
            }
            struct headerstruct* badme=block->next;
           
            

            munmap(block, block->size + META_SIZE);
             if(block==base){base=badme;}
             else{
            current->next=badme;}
            return ;


        }
    // Coalesce with the next block if it's free
    if (block->next && block->next->free) {
        block->size += META_SIZE + block->next->size;  // Merge the sizes
        block->next = block->next->next;  // Skip over the next block
    }

    // Coalesce with the previous block if it's free
    if (block != base) {
        struct headerstruct* current = base;
        while (current && current->next != block) {
            current = current->next;  // Find the block just before the current block
        }

        if (current && current->free) {
            current->size += META_SIZE + block->size;  // Merge sizes
            current->next = block->next;  // Skip over the current block
        }
    }

    // For blocks allocated with mmap, unmap them when they are freed
    // if (block->size >= MMAP_THRESHOLD && (block->mmapalloc==1)) {
    //     struct headerstruct* current = base;
    //     while (current && current->next != block) {
    //         current = current->next;  // Find the block just before the current block
    //     }
    //     struct headerstruct* badme=block->next;

        

    //     munmap(block, block->size + META_SIZE);
    //     current->next=badme;


    // }
}

// // Custom realloc implementation
// void* my_realloc(void* ptr, size_t size) {
//     if (!ptr) {
//         // If ptr is NULL, realloc behaves like malloc
//         return my_malloc(size);
//     }

//     
// }

// // Function to allocate memory using mmap
// void* my_malloc(size_t size) {
//     // Your implementation of my_malloc goes here
// }

// // Function to allocate and initialize memory to zero using mmap
// void* my_calloc(size_t nelem, size_t size) {
//     // Your implementation of my_calloc goes here
// }

// // Function to release memory allocated using my_malloc and my_calloc
// void my_free(void* ptr) {
//     // Your implementation of my_free goes here
// }



