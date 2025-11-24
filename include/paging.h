// Paging Header
// 4KB paging implementation with identity mapping

#ifndef PAGING_H
#define PAGING_H

#include <stdint.h>

// Page directory and table entry flags
#define PAGE_PRESENT    0x1     // Page is present in memory
#define PAGE_WRITE      0x2     // Page is writable
#define PAGE_USER       0x4     // Page is accessible from user mode

// Page table/directory entry structure
typedef uint32_t page_entry_t;

// Page directory and table structures
typedef struct {
    page_entry_t entries[1024];
} __attribute__((aligned(4096))) page_directory_t;

typedef struct {
    page_entry_t entries[1024];
} __attribute__((aligned(4096))) page_table_t;

// Initialize paging with identity mapping of first 4MB
void paging_init();

// Map a virtual address to a physical address
void map_page(uint32_t virtual_addr, uint32_t physical_addr, uint32_t flags);

// Unmap a virtual address
void unmap_page(uint32_t virtual_addr);

// Enable paging
void enable_paging();

#endif // PAGING_H
