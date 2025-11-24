// Physical Memory Manager (PMM) Implementation
// Bitmap-based allocator for 4KB pages

#include "pmm.h"

// External print functions
extern void print(const char* str);
extern void print_hex(unsigned int num);

// Bitmap to track page allocation (1 = used, 0 = free)
static uint8_t memory_bitmap[BITMAP_SIZE];

// Statistics
static uint32_t free_pages = 0;
static uint32_t used_pages = 0;

// Helper: Set a bit in the bitmap
static void bitmap_set(uint32_t bit) {
    uint32_t byte = bit / 8;
    uint32_t offset = bit % 8;
    memory_bitmap[byte] |= (1 << offset);
}

// Helper: Clear a bit in the bitmap
static void bitmap_clear(uint32_t bit) {
    uint32_t byte = bit / 8;
    uint32_t offset = bit % 8;
    memory_bitmap[byte] &= ~(1 << offset);
}

// Helper: Test if a bit is set
static int bitmap_test(uint32_t bit) {
    uint32_t byte = bit / 8;
    uint32_t offset = bit % 8;
    return (memory_bitmap[byte] & (1 << offset)) != 0;
}

// Helper: Find first free page
static uint32_t find_free_page() {
    for (uint32_t i = 0; i < TOTAL_PAGES; i++) {
        if (!bitmap_test(i)) {
            return i;
        }
    }
    return 0xFFFFFFFF;  // No free pages
}

// Initialize physical memory manager
void pmm_init() {
    // Clear bitmap (all pages free initially)
    for (uint32_t i = 0; i < BITMAP_SIZE; i++) {
        memory_bitmap[i] = 0;
    }
    
    // Reserve first 1MB for kernel and BIOS
    // This includes: IVT, BIOS data, VGA memory, kernel code
    uint32_t reserved_pages = (1024 * 1024) / PAGE_SIZE;  // 256 pages
    
    for (uint32_t i = 0; i < reserved_pages; i++) {
        bitmap_set(i);
        used_pages++;
    }
    
    free_pages = TOTAL_PAGES - reserved_pages;
    
    print("PMM initialized\n");
    print("Total memory: ");
    print_hex(MEMORY_SIZE);
    print(" bytes (");
    print_hex(MEMORY_SIZE / (1024 * 1024));
    print(" MB)\n");
    
    print("Page size: ");
    print_hex(PAGE_SIZE);
    print(" bytes\n");
    
    print("Total pages: ");
    print_hex(TOTAL_PAGES);
    print("\n");
    
    print("Free pages: ");
    print_hex(free_pages);
    print("\n");
    
    print("Reserved pages: ");
    print_hex(reserved_pages);
    print(" (first 1MB)\n");
}

// Allocate a physical page
uint32_t pmm_alloc() {
    uint32_t page = find_free_page();
    
    if (page == 0xFFFFFFFF) {
        print("PMM: Out of memory!\n");
        return 0;
    }
    
    bitmap_set(page);
    free_pages--;
    used_pages++;
    
    // Return physical address
    return page * PAGE_SIZE;
}

// Free a physical page
void pmm_free(uint32_t addr) {
    // Check alignment
    if (addr % PAGE_SIZE != 0) {
        print("PMM: Invalid address (not page-aligned)\n");
        return;
    }
    
    uint32_t page = addr / PAGE_SIZE;
    
    // Check bounds
    if (page >= TOTAL_PAGES) {
        print("PMM: Invalid address (out of bounds)\n");
        return;
    }
    
    // Check if page is actually allocated
    if (!bitmap_test(page)) {
        print("PMM: Warning - freeing already free page\n");
        return;
    }
    
    // Don't allow freeing reserved pages (first 1MB)
    if (page < 256) {
        print("PMM: Cannot free reserved page\n");
        return;
    }
    
    bitmap_clear(page);
    free_pages++;
    used_pages--;
}

// Get number of free pages
uint32_t pmm_get_free_pages() {
    return free_pages;
}

// Get number of used pages
uint32_t pmm_get_used_pages() {
    return used_pages;
}

// Get total number of pages
uint32_t pmm_get_total_pages() {
    return TOTAL_PAGES;
}
