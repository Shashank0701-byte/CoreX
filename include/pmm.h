// Physical Memory Manager (PMM) Header
// Bitmap-based physical memory allocator

#ifndef PMM_H
#define PMM_H

#include <stdint.h>

// Memory constants
#define PAGE_SIZE 4096              // 4KB pages
#define MEMORY_SIZE (16 * 1024 * 1024)  // 16MB total memory
#define TOTAL_PAGES (MEMORY_SIZE / PAGE_SIZE)
#define BITMAP_SIZE (TOTAL_PAGES / 8)   // 1 bit per page

// Initialize physical memory manager
void pmm_init();

// Allocate a physical page (returns physical address)
uint32_t pmm_alloc();

// Free a physical page
void pmm_free(uint32_t addr);

// Get memory statistics
uint32_t pmm_get_free_pages();
uint32_t pmm_get_used_pages();
uint32_t pmm_get_total_pages();

#endif // PMM_H
