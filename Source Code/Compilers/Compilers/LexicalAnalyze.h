#pragma once
#ifndef LexicalAnalyze
#define LexicalAnalyze

#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <string>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <algorithm>
#include <unordered_map>
#include <windows.h>
using namespace std;

#define MAXLEN 1024
typedef enum {
    NOTYP = 0, IDSY, NUM, STR, CH, CONSTSY, INTSY, CHARSY, IFSY, VOIDSY, MAIN,
    WHILESY, SWITCHSY, CASESY, DEFAULTSY, PRINTFSY, SCANFSY, RETURNSY, ELINE, ENDF,
    PLUS = 100, MINUS, MUL, DIV, LPAR, RPAR, LSBR, RSBR, LBRA, RBRA, COMMA,
    SEMI, COL, DOUQ, SINQ, ASSIGN, BGT, BLT, BLE, BGE, BEQ, NEQ
} Symbol;
typedef struct {
    Symbol sym; //enum type symbol
    string key=""; // Raw code string identified
    string strsym="NOTYP"; // str type symbol
    int num = 0; //only meaningful when sym is NUM
} Enum2str;
typedef struct {
    string tmpsym;
    string tmptoken;
    int row;
    int col;
    string line;
    string tempmsg;
    int msgno=0; //TODO: error msgs further implemention
} Errormsg;

Enum2str reserver(), lexicalanalyze(vector<string>::const_iterator&);
#endif // !LexicalAnalyze
