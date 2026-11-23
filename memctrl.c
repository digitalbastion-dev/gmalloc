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
#define FIRST 1
#define NOFIRST 0
#define PAGE_SIZE sysconf(_SC_PAGESIZE);

typedef struct config_os{
    size_t page_size;
    size_t cash1_size;
}config_os;

typedef struct block_t{
    size_t size;
    bool is_used;
    struct block_t* next_block;
}block_t;

static void* all_head = NULL;
static block_t* head = NULL;
static config_os config;

static size_t align_up(size_t size, size_t alignment){ //функция выравнивания
    if(size > SIZE_MAX - (alignment-1)) return 0;
    return (size+(alignment-1)) & ~(alignment-1);
}

static void* os_alloc(size_t size){
    if(size == 0) return NULL; //
    //long page_size = sysconf(_SC_PAGESIZE); // sysconf дает размер страницы ОС
    if(config.page_size <= 0) return NULL;
    size = align_up(size, config.page_size); //выравниваем по page_size
    if(size == 0) return NULL;
    all_head = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if(all_head == MAP_FAILED) return NULL;
    return all_head;
}

//static int os_free(void* ptr, size_t size){
//    if(ptr == NULL) return NULL;
//    long page_size = sysconf(_SC_PAGESIZE); // sysconf дает размер страницы ОС
//    if(page_size <= 0) return NULL;
//    size = align_up(size, (size_t)page_size); //выравниваем по page_size
//    if(size == 0) return NULL;
//    return munmap(ptr, size);
//}



void* block_create(size_t user_size){
    head = all_head;
    head->size = user_size;
    head->is_used = true;
    return head+1;
}

int conf_func(){
    long page_size  = sysconf(_SC_PAGESIZE);
    //void* head = os_alloc(size, page_size); // думаю лучше передавать хед ему и вызывать аллокацию отдельно
    if(page_size <= 0) return -1;
    long cache_line = sysconf(_SC_LEVEL1_DCACHE_LINESIZE);
    if(cache_line <= 0) return -1;
    config.page_size = page_size;
    config.cash1_size = cache_line;
}



void* gmalloc(size_t size){
        int err = conf_func(); // берет конфиг ос
        if(err == -1){
            write(2, "conf_func ERROR\n", 16);
        }
        void* serr = os_alloc(size); // аллоцирует страницу | ТУТ СЕГФОЛТ!!!1
        if(serr == NULL){
            write(2, "os_alloc ERROR\n", 16);
        }
        return block_create(size); // создает первый блок 
}