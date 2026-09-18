#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define COMP_PTR(state, tape_char) (state * 256 + tape_char * 2)

int main (void) {
    char* mem = malloc(256 * 1024);
    memset(mem, 0, 256 * 1024);
    
    mem[COMP_PTR(2, 0)] = 0;
    
    for (int i = 1; i < 128; i++) {
        if (i >= 97 && i <= 122) { // lowercase latin letter, to upper
            mem[COMP_PTR(2, i)] = 0b10000000 | i ^ 0b00100000;
        } else { // preserve and move on
            mem[COMP_PTR(2, i)] = 0b10000000 | i;
        }
        mem[COMP_PTR(2, i) + 1] = 2;
    }

    char* tape = mem + (256 * 256);
    strcpy(tape, "This is a test of the capitalization thing.\nIt SHOULD work \"without\" issue.\n");

    for (int i = 0; i < 128; i++) {
        printf("State 2, tape char %u: %u / %u\n", i, mem[COMP_PTR(2, i)] & 0xff, mem[COMP_PTR(2, i) + 1] & 0xff);
    }
    printf("\nTape:\n%s\n\n", tape);

    FILE *file = fopen("capitalizer.t256", "w");
    fwrite(mem, 256 * 1024, 1, file);
    fclose(file);

    return 0;
}