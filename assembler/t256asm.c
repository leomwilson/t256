#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

struct lexedSymbol {
    char type; // 0 = two-digit hex, 1 = comma, 2 = semicolon, 3 = arrow, 4 = direction, 5 = blank;
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
    struct parsedLine* next;
};

char parseTwoDigitHex (char a, char b) {
    // TODO: parse
    return 5;
}

char isHexDigit(char c) {
    return (c >= 48 && c <= 57) || (c >= 65 && c <= 70) || (c >= 97 && c <= 102);
}

int main(int argc, char** argv) {
    if (argc > 2) {
        return 3;
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
        if (state = 0 && (c == ';' || c == ',')) {
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

    initialSymbol = *initialSymbol.next; // ignore blank at beginning

    fclose(infile);

    return 0;
}