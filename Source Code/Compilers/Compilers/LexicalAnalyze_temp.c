#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h> 
#include <ctype.h>
#include <string.h>

# define MAXLEN 1024 // Max Identifier Length
//# define MAXLINE 1024 // Max line length of source file
// Global Settings
typedef enum {NOTYP=0, IDSY, NUM, STR, CH, CONSTSY, INTSY, CHARSY, IFSY, VOIDSY, MAIN, 
              WHILESY, SWITCHSY, CASESY, DEFAULTSY, PRINTFSY, SCANFSY, RETURNSY, 
              PLUS=100, MINUS, MUL, DIV, LPAR, RPAR, LSBR, RSBR, LBRA, RBRA, COMMA,
              SEMI, COL, DOUQ, SINQ, ASSIGN, BGT, BLT, BLE, BGE, BEQ, NEQ} Symbol;
typedef const struct {
    Symbol sym;
    const char *key;
    const char *str;
} enum2str;
static enum2str lex_conversion[] = {
    {ASSIGN, "=", "ASSIGN"},
    {BEQ, "==", "BEQ"},
    {NEQ, "!=", "NEQ"},
    {BGE, ">=", "BGE"},
    {BGT, ">", "BGT"},
    {BLE, "<=", "BLE"},
    {BLT, "<", "BLT"},
    {CASESY, "case", "CASESY"},
    {CHARSY, "char", "CHARSY"},
    {COL, ":", "COL"},
    {COMMA, ",", "COMMA"},
    {CONSTSY, "const", "CONSTSY"},
    {DEFAULTSY, "default", "DEFAULTSY"},
    {DIV, "/", "DIV"},
    {DOUQ, "\"", "DOUQ"},
    {IFSY, "if", "IFSY"},
    {INTSY, "int", "INTSY"},
    {LBRA, "{", "LBRA"},
    {LPAR, "(", "LPAR"},
    {LSBR, "[", "LSBR"},
    {MAIN, "main", "MAIN"},
    {MINUS, "-", "MINUS"},
    {MUL, "*", "MUL"},
    {PLUS, "+", "PLUS"},
    {PRINTFSY, "printf", "PRINTFSY"},
    {RBRA, "}", "RBRA"},
    {RETURNSY, "return", "RETURNSY"},
    {RPAR, ")", "RPAR"},
    {RSBR, "]", "RSBR"},
    {SCANFSY, "scanf", "SCANFSY"},
    {SEMI, ";", "SEMI"},
    {SINQ, "'", "SINQ"},
    {SWITCHSY, "switch", "SWITCHSY"},
    {VOIDSY, "void", "VOIDSY"},
    {WHILESY, "while", "WHILESY"}
};
FILE* input, output;
char* lex_file;
int * lex_lineseg;
struct {
    int len;
    char str[MAXLEN];
} lex_token;// Store identified word and string data
Symbol lex_sym; // Store current identified word mnemonic symbol
int lex_number; // Store integers identified
int lex_readc;  // file ptr

// Functions Declatation 
void clearToken(), catToken(char c);
int compare(const void *a, const void *b);
enum2str *reserver(), *LexicalAnalyze();
// TODO: Move above into .h and add prefix to these global vars


void main() {
    int wordcount = 0, linecount = 0, linetemp = 0, token_count = 0;
    char filename[200];
    enum2str* temp_output;
    printf("Please input the source file path\n");
    gets("%s",&filename);
    if ((input = fopen(filename, "r")) == NULL) {
        printf("Cannot open file %s", filename);
        return;
    }
    while (!feof(input)) {
        if (fgetc(input) == '\n') {
            linecount++;
        }
        wordcount++;
        printf("%c",input);
    }
    fseek(input, 0, SEEK_SET);
    lex_file = (char*)malloc(sizeof(char)*(wordcount+1));
    lex_lineseg = (int*)malloc(sizeof(int)*(linecount+1));
    for (int i = 0; i < wordcount; i++) {
        fgets(input, "%c", lex_file+i);
        if (lex_file[i] == '\n') {
            lex_lineseg[linetemp++] = i;
        }
    }
    fclose(input);
    printf("Source file loaded\n");
    lex_readc = 0; // File ptr initialize
    // Initialize reserver enum
    qsort(lex_conversion, sizeof(lex_conversion) / sizeof(enum2str), sizeof(enum2str), compare);

    while (lex_readc < wordcount) {
        temp_output = LexicalAnalyze();
        if (temp_output->sym==NOTYP) {
            // TODO: Error()
            printf("Cannot identify input!");
            continue;
        }
        token_count++;
        // if tempoutput is NUM then integer will be stored in lexnumber
        // All you need is temp_output and lexnumber afterwards
        if (temp_output->sym == CH) 
            printf("%d %s \'%s\'\n", token_count, temp_output->str, temp_output->key);
        else if (temp_output->sym == STR)
            printf("%d %s \"%s\"\n", token_count, temp_output->str, temp_output->key);
        else
            printf("%d %s %s\n", token_count, temp_output->str, temp_output->key);
    }
    return;
}

enum2str* LexicalAnalyze() {
    clearToken();
    while (isspace(lex_file[lex_readc])) {
        lex_readc++;
    }
    if (isalpha(lex_file[lex_readc])||lex_file[lex_readc] == '_') {
        while (isalnum(lex_file[lex_readc]) || lex_file[lex_readc] == '_') {
            catToken(lex_file[lex_readc++]);
        }
        return reserver();
    }
    else if (isdigit(lex_file[lex_readc])) {
        while (isdigit(lex_file[lex_readc])) {
            catToken(lex_file[lex_readc++]);
        }
        lex_number = atoi(lex_token.str);
        return &(enum2str) {NUM, lex_token.str, "NUM"};
    }
    else if (lex_file[lex_readc]=='>' || lex_file[lex_readc] == '<' || 
             lex_file[lex_readc] == '=' || lex_file[lex_readc] == '!') {
        if (lex_file[lex_readc + 1] == '=') {
            catToken(lex_file[lex_readc++]);
            catToken(lex_file[lex_readc++]);
            return reserver();
        }
        else if (lex_file[lex_readc] == '!') {
            //TODO:error
            printf("! must followed by =");
            lex_readc++;
            return &(enum2str) {NEQ, "!=", "NEQ"};
        }
        else {
            // single relation or equal
            catToken(lex_file[lex_readc++]);
            return reserver();
        }
    }
    else if (lex_file[lex_readc] == '\'') {
        lex_readc++;
        if (isalnum(lex_file[lex_readc]) || lex_file[lex_readc] == '+' ||
            lex_file[lex_readc] == '-' || lex_file[lex_readc] == '*' ||
            lex_file[lex_readc] == '/' || lex_file[lex_readc] == '_') {
            catToken(lex_file[lex_readc++]);
        }
        else {
            printf("Not a valid char\n");
            //TODO: error
        }
        if (lex_file[lex_readc] != '\'') {
            //TODO: error '' ""
            printf("Missing Single Quote/Should use \" instead\n");
        }
        lex_readc++;
        return &(enum2str) {CH, lex_token.str, "CH"};
    }
    else if (lex_file[lex_readc] == '\"') {
        lex_readc++;
        while (lex_file[lex_readc] != '\"') {
            catToken(lex_file[lex_readc++]);
        }
        lex_readc++;
        return &(enum2str) {STR, lex_token.str, "STR"};
    }
    else {
        // Identify all the rest like +-*/;:,(){}[]
        lex_readc++;
        return reserver();
    }
}

// Flush identified token
void clearToken() {
    lex_token.str[0] = '\0';
    lex_token.len = 0;
    return;
}

// Concat input char into token
void catToken(char c) {
    if (lex_token.len >= MAXLEN) {
        printf("Token exceeds maxlen\n");
        //TODO: error
        return;
    }
    else {
        lex_token.str[lex_token.len++] = c;
        lex_token.str[lex_token.len] = '\0';
        return;
    }
}

int compare(const void *a, const void *b) {
    return (strcmp((*(enum2str*)a).key, (*(enum2str*)b).key ));
}

enum2str* reserver() {
    enum2str temp = { IDSY, lex_token.str, "IDSY" };
    enum2str* pointer = (enum2str*)bsearch(&temp, lex_conversion, sizeof(lex_conversion)/sizeof(enum2str), sizeof(enum2str), compare);
    if (pointer == NULL) {
        pointer = &temp;
        // return IDSY as temporary result, 
        // leave for grammer analyze to decide whether it is a function or var/const
    }
    return pointer;
}
