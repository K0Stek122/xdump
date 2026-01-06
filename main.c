#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

unsigned long get_file_size(FILE* fptr) {
    if (!fptr) {
        return 0;
    }
    // This function will cause problems if it's called in the middle of reading the file, since it will.
    // set the fseek stream buffer back to the beginning of the file.
    fseek(fptr, 0, SEEK_END); //fseek moves the stream to a location in the file.
    unsigned long size = ftell(fptr);
    fseek(fptr, 0, SEEK_SET); // This moves the stream of the file back to the beginning so that read_file will still work correctly.
    rewind(fptr);
    return size;
}

bool read_all_data_from_file(FILE* fptr, unsigned char* output, size_t output_size) {
    if (!fptr) {
        return false;
    }
    
    size_t bytes;
    size_t total = 0;
    
    while ((bytes = fread(output + total, 1, output_size - total, fptr)) > 0) {
        total += bytes;
        if (total == output_size) {
            break; //buffer full. Prevents buffer overflow
        }
    }

    return true;
}

int main(int argc, const char* argv[]) {
    if (argc != 2) {
        printf("usage: xdump <file>\n");
        return 1;
    }

    // Get file size with the temporary pointer
    FILE* fptr;
    fptr = fopen(argv[1], "rb");
    if (!fptr) {
        printf("error: provided file does not exist.\n");
        return 1;
    }

    long file_size = get_file_size(fptr);
    unsigned char* file_input = malloc(file_size);
    if (!file_input) {
        printf("error: failed to initialise the heap.\n");
        fclose(fptr);
        return 1;
    }

    if (!read_all_data_from_file(fptr, file_input, file_size)) {
        printf("File does not exist!\n");
        return 1;
    }
    
    fclose(fptr);
    
    // Now the fun begins :)
    for (size_t loc = 0; loc < file_size; loc += 10) {
        printf("\n%08lX: ", loc);
        for (size_t hex = loc; hex < (loc + 10); hex++) {
            printf("%02X ", file_input[hex]);
        }
        
        printf(" | ");

        for (size_t ascii = loc; ascii < (loc + 10); ascii++) {
            int ascii_num = (int)file_input[ascii];
            if (ascii_num > 32 && ascii_num < 127) { // range of printable characters
                printf("%c", file_input[ascii]); 
            }
            else {
                printf(".");
            }
        }
    }
    printf("\n");


    free(file_input);
    return 0;
}
