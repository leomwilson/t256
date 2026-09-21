#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

const char FLAG_DEBUG = 0b10000000;
const char ASCII = 0b01111111;

void print_tape(char* ptr) {
    while (*ptr) {
        putchar(*ptr);
        ptr++;
    }
}

int main (int argc, char** argv) {
    char* fname = argv[argc - 1];
    char flags = 0;

    // flags
    for (int i = 0; i < argc - 1; i++) {
        if (strcmp(argv[i], "-d") == 0) {
            flags = flags | FLAG_DEBUG;
        }
    }

    /** MEMORY LAYOUT:
      * For each state (x256 states:)
      * | next tape (L/R + 7-bit ASCII) | next state (8 bits) | * 128 (for each ASCII input)
      * ^ 64kb total (256 * 128 * 2 / 1024)
      * Tape (192kb of single chars)
      */
    char* mem = malloc(256 * 1024);
    memset(mem, 0, 256 * 1024);
    char* tape_initial = mem + (256 * 256);
    const char* TAPE_MAX = mem + (256 * 1024);

    // read in instruction portion
    FILE* file = fopen(fname, "r");
    fread(mem, 256 * 256, 1, file);
    fclose(file);

    // read in tape
    // TODO: add more tape options
    char ch;
    char* tape_write_ptr = tape_initial;
    while (read(STDIN_FILENO, &ch, 1) > 0 && tape_write_ptr < TAPE_MAX) {
        *tape_write_ptr = ch;
        tape_write_ptr++;
    }

    if (flags & FLAG_DEBUG) {
        fprintf(stderr, "Initial tape:\n");
        print_tape(tape_initial);
    }

    char* ptr = tape_initial;
    char state = 2;

    while (1) {
        char tape_char = *ptr & ASCII;
        char next_tape = mem[state * 256 + tape_char * 2];
        *ptr = next_tape & ASCII;
        if ((next_tape ^ ASCII) && ptr < TAPE_MAX) { // right
            ptr++;
        } else if (ptr > tape_initial) { // left
            ptr--;
        }
        state = mem[state * 256 + tape_char * 2 + 1];

        if (state == 0) {
            fprintf(stderr, "ACCEPT. Tape:\n");
            print_tape((next_tape ^ ASCII) ? ptr : tape_initial);
            free(mem);
            return 0;
        } else if (state == 1) {
            fprintf(stderr, "REJECT. Tape:\n");
            print_tape((next_tape ^ ASCII) ? ptr : tape_initial);
            free(mem);
            return 1;
        }
    }

    return 3;
}