#include <raylib.h>
#include <stddef.h>   // size_t, NULL, max_align_t
#include <stdint.h>   // uintptr_t, uint8_t
#include <stdbool.h>  // bool
#include <string.h>   // memset, memcpy (для calloc / realloc)
#include <unistd.h>   // sbrk, sysconf (Linux / POSIX)
#include <sys/mman.h> // mmap, munmap (Linux / POSIX)
#include <pthread.h>  // pthread_mutex_t (опционально, для потокобезопасности)
#include "memctrl.h"

//void *mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset);
//int munmap(void *addr, size_t length);

#define PAGE_SIZE sysconf(_SC_PAGESIZE);

typedef struct block_t{
    size_t size;
    bool is_used;
    struct block_t* next_block;
}block_t;

static block_t* head = NULL;

static size_t align_up(size_t size, size_t alignment){ //функция выравнивания
    if(size > SIZE_MAX - (alignment-1)) return 0;
    return (size+(alignment-1)) & ~(alignment-1);
}

static void* os_alloc(size_t size){
    if(size == 0) return NULL; //
    long page_size = sysconf(_SC_PAGESIZE); // sysconf дает размер страницы ОС
    if(page_size <= 0) return NULL;
    size = align_up(size, (size_t)page_size); //выравниваем по page_size
    if(size == 0) return NULL;
    void* ptr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if(ptr == MAP_FAILED) return NULL;
    return ptr;
}

static int os_free(void* ptr, size_t size){
    if(ptr == NULL) return -1;
    long page_size = sysconf(_SC_PAGESIZE); // sysconf дает размер страницы ОС
    if(page_size <= 0) return -1;
    size = align_up(size, (size_t)page_size); //выравниваем по page_size
    if(size == 0) return NULL;
    return munmap(ptr, size);
}

void* arena_ctrl(size_t inc){
    return sbrk(inc);
}

void* block_create(size_t size, block_t* head, size_t user_size){
    head->size = size;
    head->is_used = true;
    char* nextadr = (char*)head + align_up(sizeof(block_t), 16) + head->size;
    head->next_block = (block_t*)nextadr;
    
}

void* gmalloc(size_t size){
    if(head == NULL){
        void* page = os_alloc(size);
        
    }
}