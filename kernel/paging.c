// Paging Implementation
// 4KB paging with identity mapping

#include "paging.h"
#include "pmm.h"

// External print functions
extern void print(const char* str);
extern void print_hex(unsigned int num);

// Page directory (aligned to 4KB)
static page_directory_t kernel_directory __attribute__((aligned(4096)));

// Page table for first 4MB (aligned to 4KB)
static page_table_t first_page_table __attribute__((aligned(4096)));

// Helper: Get page directory index from virtual address
static inline uint32_t get_pd_index(uint32_t virtual_addr) {
    return virtual_addr >> 22;  // Top 10 bits
}

// Helper: Get page table index from virtual address
static inline uint32_t get_pt_index(uint32_t virtual_addr) {
    return (virtual_addr >> 12) & 0x3FF;  // Middle 10 bits
}

// Helper: Get physical address from page entry
static inline uint32_t get_page_frame(page_entry_t entry) {
    return entry & 0xFFFFF000;  // Top 20 bits
}

// Initialize paging
void paging_init() {
    print("Initializing paging...\n");
    
    // Clear page directory
    for (int i = 0; i < 1024; i++) {
        kernel_directory.entries[i] = 0;
    }
    
    // Identity map first 4MB (0x00000000 - 0x003FFFFF)
    // This covers kernel code, VGA memory, and initial data
    for (int i = 0; i < 1024; i++) {
        uint32_t physical_addr = i * 4096;  // Each page is 4KB
        first_page_table.entries[i] = physical_addr | PAGE_PRESENT | PAGE_WRITE;
    }
    
    // Install first page table into page directory
    kernel_directory.entries[0] = ((uint32_t)&first_page_table) | PAGE_PRESENT | PAGE_WRITE;
    
    print("Identity mapped first 4MB (1024 pages)\n");
    print("Page directory at: ");
    print_hex((uint32_t)&kernel_directory);
    print("\n");
    
    print("Page table at: ");
    print_hex((uint32_t)&first_page_table);
    print("\n");
    
    // Enable paging
    enable_paging();
    
    print("Paging enabled!\n");
}

// Map a virtual address to a physical address
void map_page(uint32_t virtual_addr, uint32_t physical_addr, uint32_t flags) {
    uint32_t pd_index = get_pd_index(virtual_addr);
    uint32_t pt_index = get_pt_index(virtual_addr);
    
    // Check if page directory entry exists
    if (!(kernel_directory.entries[pd_index] & PAGE_PRESENT)) {
        // Need to create a new page table
        uint32_t new_table = pmm_alloc();
        if (new_table == 0) {
            print("Paging: Failed to allocate page table\n");
            return;
        }
        
        // Clear the new page table
        page_table_t* table = (page_table_t*)new_table;
        for (int i = 0; i < 1024; i++) {
            table->entries[i] = 0;
        }
        
        // Install page table into directory
        kernel_directory.entries[pd_index] = new_table | PAGE_PRESENT | PAGE_WRITE | flags;
    }
    
    // Get page table
    page_table_t* table = (page_table_t*)get_page_frame(kernel_directory.entries[pd_index]);
    
    // Map the page
    table->entries[pt_index] = (physical_addr & 0xFFFFF000) | PAGE_PRESENT | flags;
    
    // Flush TLB for this address
    __asm__ __volatile__("invlpg (%0)" : : "r"(virtual_addr) : "memory");
}

// Unmap a virtual address
void unmap_page(uint32_t virtual_addr) {
    uint32_t pd_index = get_pd_index(virtual_addr);
    uint32_t pt_index = get_pt_index(virtual_addr);
    
    // Check if page directory entry exists
    if (!(kernel_directory.entries[pd_index] & PAGE_PRESENT)) {
        return;  // Already unmapped
    }
    
    // Get page table
    page_table_t* table = (page_table_t*)get_page_frame(kernel_directory.entries[pd_index]);
    
    // Unmap the page
    table->entries[pt_index] = 0;
    
    // Flush TLB for this address
    __asm__ __volatile__("invlpg (%0)" : : "r"(virtual_addr) : "memory");
}

// Enable paging
void enable_paging() {
    // Load page directory into CR3
    __asm__ __volatile__(
        "mov %0, %%cr3\n"
        : : "r"(&kernel_directory)
    );
    
    // Enable paging by setting bit 31 of CR0
    uint32_t cr0;
    __asm__ __volatile__(
        "mov %%cr0, %0\n"
        "or $0x80000000, %0\n"  // Set PG bit (bit 31)
        "mov %0, %%cr0\n"
        : "=r"(cr0)
    );
}
