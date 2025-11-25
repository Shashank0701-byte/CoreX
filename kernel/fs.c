// Simple In-Memory File System Implementation
// Flat file system stored in memory

#include "fs.h"

// External print functions
extern void print(const char* str);
extern void print_hex(unsigned int num);

// File system storage
// Store at fixed memory location 0x20000 (128KB) to avoid increasing kernel binary size
#define FS_MEMORY_START 0x20000
static file_t* files = (file_t*)FS_MEMORY_START;
static int fs_initialized = 0;

// String utility functions
static int strlen(const char* str) {
    int len = 0;
    while (str[len] != '\0') len++;
    return len;
}

static int strcmp(const char* s1, const char* s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(unsigned char*)s1 - *(unsigned char*)s2;
}

static void strcpy(char* dest, const char* src) {
    while (*src) {
        *dest++ = *src++;
    }
    *dest = '\0';
}

static void memcpy(void* dest, const void* src, uint32_t n) {
    uint8_t* d = (uint8_t*)dest;
    const uint8_t* s = (const uint8_t*)src;
    while (n--) {
        *d++ = *s++;
    }
}

// Initialize file system
void fs_init() {
    for (int i = 0; i < MAX_FILES; i++) {
        files[i].used = 0;
        files[i].name[0] = '\0';
        files[i].size = 0;
    }
    fs_initialized = 1;
    print("File system initialized\n");
}

// Find file by name
static int find_file(const char* filename) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (files[i].used && strcmp(files[i].name, filename) == 0) {
            return i;
        }
    }
    return -1;
}

// Find free file slot
static int find_free_slot() {
    for (int i = 0; i < MAX_FILES; i++) {
        if (!files[i].used) {
            return i;
        }
    }
    return -1;
}

// Create a file with content
int fs_create(const char* filename, const char* content) {
    if (!fs_initialized) {
        print("FS: Not initialized\n");
        return -1;
    }
    
    // Check if file already exists
    if (find_file(filename) >= 0) {
        print("FS: File already exists\n");
        return -1;
    }
    
    // Check filename length
    if (strlen(filename) >= MAX_FILENAME) {
        print("FS: Filename too long\n");
        return -1;
    }
    
    // Check content size
    uint32_t content_size = strlen(content);
    if (content_size > MAX_FILE_SIZE) {
        print("FS: File too large\n");
        return -1;
    }
    
    // Find free slot
    int slot = find_free_slot();
    if (slot < 0) {
        print("FS: No free slots\n");
        return -1;
    }
    
    // Create file
    file_t* file = &files[slot];
    strcpy(file->name, filename);
    file->size = content_size;
    memcpy(file->data, content, content_size);
    file->used = 1;
    
    return 0;
}

// Read a file
int fs_read(const char* filename, char* buffer, uint32_t size) {
    if (!fs_initialized) {
        print("FS: Not initialized\n");
        return -1;
    }
    
    int slot = find_file(filename);
    if (slot < 0) {
        print("FS: File not found\n");
        return -1;
    }
    
    file_t* file = &files[slot];
    uint32_t read_size = (size < file->size) ? size : file->size;
    
    memcpy(buffer, file->data, read_size);
    buffer[read_size] = '\0';
    
    return read_size;
}

// List all files
void fs_list() {
    if (!fs_initialized) {
        print("FS: Not initialized\n");
        return;
    }
    
    int count = 0;
    print("\nFiles:\n");
    print("  Name                Size (bytes)\n");
    print("  --------------------------------\n");
    
    for (int i = 0; i < MAX_FILES; i++) {
        if (files[i].used) {
            print("  ");
            print(files[i].name);
            
            // Pad to 20 characters
            int name_len = strlen(files[i].name);
            for (int j = name_len; j < 20; j++) {
                print(" ");
            }
            
            print_hex(files[i].size);
            print("\n");
            count++;
        }
    }
    
    if (count == 0) {
        print("  (no files)\n");
    }
    
    print("\nTotal files: ");
    print_hex(count);
    print("\n");
}

// Delete a file
int fs_delete(const char* filename) {
    if (!fs_initialized) {
        print("FS: Not initialized\n");
        return -1;
    }
    
    int slot = find_file(filename);
    if (slot < 0) {
        print("FS: File not found\n");
        return -1;
    }
    
    files[slot].used = 0;
    files[slot].name[0] = '\0';
    files[slot].size = 0;
    
    return 0;
}

// Get file size
int fs_size(const char* filename) {
    if (!fs_initialized) {
        return -1;
    }
    
    int slot = find_file(filename);
    if (slot < 0) {
        return -1;
    }
    
    return files[slot].size;
}
