// Simple In-Memory File System Header
// Flat file system with basic operations

#ifndef FS_H
#define FS_H

#include <stdint.h>

// File system constants
#define MAX_FILES 16
#define MAX_FILENAME 32
#define MAX_FILE_SIZE 1024

// File structure
typedef struct {
    char name[MAX_FILENAME];
    uint32_t size;
    uint8_t data[MAX_FILE_SIZE];
    int used;
} file_t;

// Initialize file system
void fs_init();

// Create a file with content
int fs_create(const char* filename, const char* content);

// Read a file
int fs_read(const char* filename, char* buffer, uint32_t size);

// List all files
void fs_list();

// Delete a file
int fs_delete(const char* filename);

// Get file size
int fs_size(const char* filename);

#endif // FS_H
