#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>

#define COMP_PTR(state, tape_char) (state * 256 + tape_char * 2)

const char ASCII = 0b01111111;
struct lexedSymbol {
    char type; // 0 = two-digit hex, 1 = comma, 2 = semicolon, 3 = arrow, 4 = direction, 5 = blank
    char data;
    int line;
    struct lexedSymbol* next;
};

struct parsedLine {
    char fromState;
    char tapeChar;
    char toState;
    char newChar;
    char movement; // 0 = left, 1 = right
    int line; // line in file, for debugging
    struct parsedLine* next;
};

char parseSingleHexDigit (char c) {
    // NOTE: assumes valid hex digit
    if (c <= 57) {
        return c - 48; // '0' -> 0, etc
    }
    c &= 0b11011111; // force uppercase
    return c - 55; // 'A' -> 10, etc
}

char parseTwoDigitHex (char a, char b) {
    return parseSingleHexDigit(a) << 4 & parseSingleHexDigit(b);
}

char isHexDigit(char c) {
    return (c >= 48 && c <= 57) || (c >= 65 && c <= 70) || (c >= 97 && c <= 102);
}

int main(int argc, char** argv) {
    if (argc == 0 || argc > 2) {
        return 10;
    }

    char* infname;
    infname = argv[0];
    char outfname[strlen(infname) + 5];
    
    if (argc == 2) {
        strncpy(outfname, argv[1], sizeof(outfname) - 1);
    } else {
        strcpy(outfname, infname);
        strcat(outfname, ".t256");
    }

    FILE* infile = fopen(infname, "r");
    char c = 1;
    char storedChar;
    int line = 0;
    char state = 0; // 0 = ready, 1 = number, 2 = arrow, 3 = comment
    struct lexedSymbol initialSymbol;
    initialSymbol.type = 5;
    initialSymbol.line = 0;
    struct lexedSymbol prevSymbol = initialSymbol;
    while (c != EOF) {
        c = (char) fgetc(infile);

        // comments
        if (state == 3 && c == '\n') {
            state = 0;
            // NOTE: still triggers whitespace and newline handler later,
            // since it does not continue
        } else if (state == 3) {
            continue;
        } else if (state != 3 && c == '#') {
            state = 4;
            continue;
        }

        // whitespace
        if (c == '\n') { line++; }
        if (isspace(c)) { continue; }

        // number
        if (state == 0 && isHexDigit(c)) {
            storedChar = c;
            state = 1;
            continue;
        }
        if (state == 1 && isHexDigit(c)) {
            struct lexedSymbol curSymbol;
            curSymbol.type = 0;
            curSymbol.line = line;
            curSymbol.data = parseTwoDigitHex(storedChar, c);
            prevSymbol.next = &curSymbol;
            prevSymbol = curSymbol;
            storedChar = 0;
            state = 0;
            continue;
        }

        // semicolons and commas
        if (state == 0 && (c == ';' || c == ',')) {
            struct lexedSymbol curSymbol;
            curSymbol.type = (c == ';') ? 2 : 1;
            curSymbol.line = line;
            prevSymbol.next = &curSymbol;
            prevSymbol = curSymbol;
        }

        // arrows
        if (state == 0 && c == '-') {
            state = 2;
            continue;
        }
        if (state == 2 && c == '>') {
            struct lexedSymbol curSymbol;
            curSymbol.type = 3;
            curSymbol.line = line;
            prevSymbol.next = &curSymbol;
            prevSymbol = curSymbol;
            state = 0;
            continue;
        }

        // not handled, error
        fprintf(stderr, "Lexer error on line %d: Incorrect syntax.\n", line);
        fclose(infile);
        return 1;
    }

    if (state != 0) {
        fprintf(stderr, "Lexer error on line %d: Unterminated symbol.\n", line);
        fclose(infile);
        return 1;
    }

    fclose(infile);

    struct parsedLine initialLine;
    initialLine.fromState = 0;
    initialLine.tapeChar = 0;
    initialLine.toState = 0;
    initialLine.newChar = 0;
    initialLine.movement = 0;

    struct parsedLine prevLine = initialLine;
    struct lexedSymbol curSymbol = initialSymbol;

    while (curSymbol.next != NULL) {
        curSymbol = *curSymbol.next; // NOTE: skips the initial blank symbol
        struct parsedLine curLine;

        // from state
        if (curSymbol.type != 0) {
            fprintf(stderr, "Parser error on line %d: Malformed statement.\n", curSymbol.line);
            return 2;
        }
        curLine.fromState = curSymbol.data;
        curLine.line = curSymbol.line;
        if (curSymbol.next == NULL) {
            fprintf(stderr, "Parser error on line %d: Incomplete statement.\n", curSymbol.line);
            return 2;
        }
        curSymbol = *curSymbol.next;
        
        // comma
        if (curSymbol.type != 1) {
            fprintf(stderr, "Parser error on line %d: Malformed statement.\n", curSymbol.line);
            return 2;
        }
        if (curSymbol.next == NULL) {
            fprintf(stderr, "Parser error on line %d: Incomplete statement.\n", curSymbol.line);
            return 2;
        }
        curSymbol = *curSymbol.next;

        // tape char
        if (curSymbol.type != 0) {
            fprintf(stderr, "Parser error on line %d: Malformed statement.\n", curSymbol.line);
            return 2;
        }
        curLine.tapeChar = curSymbol.data;
        if (curSymbol.next == NULL) {
            fprintf(stderr, "Parser error on line %d: Incomplete statement.\n", curSymbol.line);
            return 2;
        }
        curSymbol = *curSymbol.next;

        // arrow
        if (curSymbol.type != 3) {
            fprintf(stderr, "Parser error on line %d: Malformed statement.\n", curSymbol.line);
            return 2;
        }
        if (curSymbol.next == NULL) {
            fprintf(stderr, "Parser error on line %d: Incomplete statement.\n", curSymbol.line);
            return 2;
        }
        curSymbol = *curSymbol.next;

        // to state
        if (curSymbol.type != 0) {
            fprintf(stderr, "Parser error on line %d: Malformed statement.\n", curSymbol.line);
            return 2;
        }
        curLine.toState = curSymbol.data;
        if (curSymbol.next == NULL) {
            fprintf(stderr, "Parser error on line %d: Incomplete statement.\n", curSymbol.line);
            return 2;
        }
        curSymbol = *curSymbol.next;

        // comma
        if (curSymbol.type != 1) {
            fprintf(stderr, "Parser error on line %d: Malformed statement.\n", curSymbol.line);
            return 2;
        }
        if (curSymbol.next == NULL) {
            fprintf(stderr, "Parser error on line %d: Incomplete statement.\n", curSymbol.line);
            return 2;
        }
        curSymbol = *curSymbol.next;

        // new char
        if (curSymbol.type != 0) {
            fprintf(stderr, "Parser error on line %d: Malformed statement.\n", curSymbol.line);
            return 2;
        }
        curLine.newChar = curSymbol.data;
        if (curSymbol.next == NULL) {
            fprintf(stderr, "Parser error on line %d: Incomplete statement.\n", curSymbol.line);
            return 2;
        }
        curSymbol = *curSymbol.next;

        // comma
        if (curSymbol.type != 1) {
            fprintf(stderr, "Parser error on line %d: Malformed statement.\n", curSymbol.line);
            return 2;
        }
        if (curSymbol.next == NULL) {
            fprintf(stderr, "Parser error on line %d: Incomplete statement.\n", curSymbol.line);
            return 2;
        }
        curSymbol = *curSymbol.next;

        // movement
        if (curSymbol.type != 4) {
            fprintf(stderr, "Parser error on line %d: Malformed statement.\n", curSymbol.line);
            return 2;
        }
        curLine.movement = curSymbol.data;
        if (curSymbol.next == NULL) {
            fprintf(stderr, "Parser error on line %d: Incomplete statement.\n", curSymbol.line);
            return 2;
        }
        curSymbol = *curSymbol.next;

        // semicolon
        if (curSymbol.type != 2) {
            fprintf(stderr, "Parser error on line %d: Malformed statement.\n", curSymbol.line);
            return 2;
        }

        // add to the linked list
        prevLine.next = &curLine;
        prevLine = curLine;
    }

    struct parsedLine curLine = initialLine;

    char* mem = malloc(256 * 256);
    memset(mem, 0, 256 * 256);

    while (curLine.next != NULL) {
        curLine = *curLine.next; // NOTE: skips the initial empty line 
        mem[COMP_PTR(curLine.fromState, curLine.tapeChar)] = (curLine.newChar & ASCII) | (curLine.movement < 7);
        mem[COMP_PTR(curLine.fromState, curLine.tapeChar) + 1] = curLine.toState;
    };

    // write out to file
    FILE *file = fopen(outfname, "w");
    fwrite(mem, 256 * 256, 1, file);
    fclose(file);
    free(mem);

    return 0;
}