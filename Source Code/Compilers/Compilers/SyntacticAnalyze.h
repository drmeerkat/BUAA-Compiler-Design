#pragma once
#ifndef Syntactic
#define Syntactic

#include "LexicalAnalyze.h"
#include <iomanip>

using namespace std;

typedef struct {
    int type; //10-int or 11-char
    int max_capacity;
} Arrayinfo;
typedef struct {
    vector<pair<int, string>> paralist; // type(10-int/11-char),var_name
    bool isreturn;
    string re_type;
    string func_name;
    int varcnt;
} Funcinfo;
typedef struct {
    string level_name;
    int kind=0; // 1-array,2-func,3-const,4-simplevar,5-parameter
    int type=0; // 10-int, 11-char, 12-void //void is possible only when kind==func
    // Optional field some func/global/array have no such fields
    string var_name;
    Arrayinfo arrinfo;
    Funcinfo funcinfo;
    int num; // num and ch used for store const value
    char ch;
    int offset;
    int stat_line;
    vector<int> use_line;
    bool completed = false; 
} Tabinfo;
typedef struct {
    string level_n;
    string op;
    string op1;
    string op2;
    string op3;
    // leave empty if no use
    Tabinfo tabop1 = {}; // record result tab
    Tabinfo tabop2 = {};
    Tabinfo tabop3 = {};
} Quat;
typedef struct {
    string var_name;
    string level_name;
    bool isdirty;
    bool needwb;
    string regname;// something like $s0,$t1,$fp
    string subscript;//useful when stored value is a array value 
} Reginfo;
typedef struct {
    string details;
    int row;
} Errormsg2;
typedef vector<string>::const_iterator LinePtr;

int lex_rownum = 1, lex_colnum = 0, lex_tokencount = 0;
vector<string> lex_file;
vector<Errormsg> lex_errors;
string lex_token;
Enum2str lex_output;
// Global Settings
unordered_map<string, Enum2str> lex_conversion = {
    {"=", {ASSIGN, "=", "ASSIGN" }},
    {"==", {BEQ, "==", "BEQ" }},
    {"!=", {NEQ, "!=", "NEQ" }},
    {">=", {BGE, ">=", "BGE" }},
    {">", {BGT, ">", "BGT" }},
    {"<=", {BLE, "<=", "BLE" }},
    {"<", {BLT, "<", "BLT" }},
    {"case", {CASESY, "case", "CASESY" }},
    {"char", {CHARSY, "char", "CHARSY" }},
    {":", {COL, ":", "COL" }},
    {",", {COMMA, ",", "COMMA" }},
    {"const", {CONSTSY, "const", "CONSTSY" }},
    {"default", {DEFAULTSY, "default", "DEFAULTSY" }},
    {"/", {DIV, "/", "DIV" }},
    {"\"", {DOUQ, "\"", "DOUQ" }},
    {"if", {IFSY, "if", "IFSY" }},
    {"int", {INTSY, "int", "INTSY" }},
    {"{", {LBRA, "{", "LBRA" }},
    {"(", {LPAR, "(", "LPAR" }},
    {"[", {LSBR, "[", "LSBR" }},
    {"main", {MAIN, "main", "MAIN" }},
    {"-", {MINUS, "-", "MINUS" }},
    {"*", {MUL, "*", "MUL" }},
    {"+", {PLUS, "+", "PLUS" }},
    {"printf", {PRINTFSY, "printf", "PRINTFSY" }},
    {"}", {RBRA, "}", "RBRA" }},
    {"return", {RETURNSY, "return", "RETURNSY" }},
    {")", {RPAR, ")", "RPAR" }},
    {"]", {RSBR, "]", "RSBR" }},
    {"scanf", {SCANFSY, "scanf", "SCANFSY" }},
    {";", {SEMI, ";", "SEMI" }},
    {"'", {SINQ, "'", "SINQ" }},
    {"switch", {SWITCHSY, "switch", "SWITCHSY" }},
    {"void", {VOIDSY, "void", "VOIDSY" }},
    {"while", {WHILESY, "while", "WHILESY"}}
};
const set<Symbol> reserved_name = {CASESY, CHARSY, CONSTSY, DEFAULTSY, IFSY, INTSY, MAIN, PRINTFSY, RETURNSY, SCANFSY, SWITCHSY, VOIDSY, WHILESY};

unordered_map<char, int> special_str_ch = {
    {'#',35}, {'$',36}, {'%',37}, {'&',38}, {'.',46},
    {'?',63}, {'@',64}, {'\\',92}, {'^',94}, {'|',124}, {'~',126}
};

unordered_map<int, string> map_kind = {
    {1, "array"}, {2, "func"}, {3, "const"}, {4, "var"}, {5, "parameter"}
};

unordered_map<string, int> strmap_kind = {
    {"array", 1}, {"func", 2}, {"const", 3}, {"var", 4}, {"parameter", 5}
};

unordered_map<string, int> strmap_type = {
    {"int", 10}, {"char", 11}, {"void", 12} 
};

unordered_map<int, string> map_type = {
    {10, "int"}, {11, "char"}, {12, "void"}
};

const set<string> expop = { "+","-","*","/" };
const set<string> boolop = { "==","<",">","!=","<=",">=" };

unordered_map<string, vector<pair<string, Tabinfo>>> syntac_symtab; // symbol tab <level name, level tab <var name, var_info>>
LinePtr syntac_lineptr;
string syntac_level_name;
bool end_flag = false; // end of file flag
int syntac_tmpvar_cnt = 0, syntac_label_cnt = 0, syntac_offset = 0;//start from 0 for func

LinePtr syntac_tmp_lineptr; // used by save()/load()
int syntac_tmp_col;
Enum2str lex_tmp_output;

vector<Quat> syntac_quat; // Quatary vector
vector<string> syntac_mips; // Mips code vector
vector<pair<string, string>> syntac_tmpvar; // tmp_var<NO, levelname>
vector<int> gen_usedsreg; // stored number of sreg used by callee 0-7s

vector<pair<int, vector<string>>> gen_pararecord;//record the push para procedure func<fname, para>
vector<pair<string, int>> syntac_funcnum;//record multi function call
int funccount = 0;

vector<Errormsg2> syntac_errors;

unordered_map<string, string> syntac_strlabeltab;//string table
unordered_map<string, Reginfo> gen_regs; // add regs and initialize when first time use
unordered_map<string, vector<int>> gen_multiuse;//stroed multiple time used tmpvar

int cons_declaration(LinePtr&, Funcinfo&), var_declaration(LinePtr&, Funcinfo&), function_declaration(LinePtr&);
int paralist(LinePtr&, Funcinfo&), value_paralist(LinePtr&, Funcinfo&, int), func_call(LinePtr&, bool, string&, string&);
int func_main(LinePtr&), func_compound(LinePtr&, Funcinfo&);
int statement(LinePtr&), sta_switch(LinePtr&), sta_if(LinePtr&), sta_while(LinePtr&);
int sta_return(LinePtr&), sta_printf(LinePtr&), sta_scanf(LinePtr&), sta_assign(LinePtr&);
int expression(LinePtr&, string&, string&), condition(LinePtr&, string&), case_list(LinePtr&, const string&, const string&, const string&), def_list(LinePtr&);
int term(LinePtr&, int, string&, string&), factor(LinePtr&, string&, string&);
int enter(string&, Tabinfo, int kind, int type, Funcinfo, Arrayinfo, int);
bool isrequiredch(Enum2str, string), lookuptab(Tabinfo&, string, string, int&, set<int>, set<int>);
Enum2str getsym(LinePtr&);
void save(), load(), genQuat(string op, string, string, string, Tabinfo, Tabinfo, Tabinfo);
Quat formalizeQuat(Quat tmp);

void genMips_const(vector<Quat>::const_iterator& qiter, bool isglob);
void genMips_var(vector<Quat>::const_iterator& qiter, bool isglob);
void genMips_array(vector<Quat>::const_iterator& qiter, bool isglob);
void genMips_funcdec(vector<Quat>::const_iterator& qiter);
void genMips_call(vector<Quat>::const_iterator& qiter);
void genMips_push(vector<Quat>::const_iterator& qiter);
void genMips_exp(vector<Quat>::const_iterator& qiter);
void genMips_arraycall(vector<Quat>::const_iterator& qiter);
void genMips_assign(vector<Quat>::const_iterator& qiter);
void genMips_bool(vector<Quat>::const_iterator& qiter);
void genMips_switch(vector<Quat>::const_iterator& qiter);
void genMips_label(vector<Quat>::const_iterator& qiter, string);
void genMips_jump(vector<Quat>::const_iterator& qiter);
void genMips_ret(vector<Quat>::const_iterator& qiter);
void genMips_printf(vector<Quat>::const_iterator& qiter);
void genMips_scanf(vector<Quat>::const_iterator& qiter);
void genMips_recordreg(string, string, string, string, bool);
void genMips_findvar(string, string&, bool, vector<Quat>::const_iterator, string);
bool genMips_findreg(Reginfo& findresult, string levelname, string varname, string subscript);
string genMips_lastareguser(int aregnum, string caller);
void genMips(), scanMultiuse();
string allocateTempReg(vector<Quat>::const_iterator), gentmpvar(), genlabel();

Enum2str lexicalanalyze(vector<string>::const_iterator& iter_line) {
    while (lex_colnum >= (*iter_line).size()) {
        iter_line++;
        lex_rownum++;
        lex_colnum = 0;
        if (iter_line == lex_file.cend()) {
            return { ENDF };
        }
    }
    string line = *iter_line;
    lex_token.clear();
    while (isspace(line.data()[lex_colnum])) {
        lex_colnum++;
        if (lex_colnum == line.size())
            return { ELINE }; // multi line code is not allowed
    }
    if (isalpha(line.data()[lex_colnum]) || line.data()[lex_colnum] == '_') {
        while (isalnum(line.data()[lex_colnum]) || line.data()[lex_colnum] == '_') {
            lex_token.push_back(line.data()[lex_colnum++]);
        }
        return reserver();
    }
    else if (isdigit(line.data()[lex_colnum])) {
        while (lex_colnum != line.size() && isdigit(line.data()[lex_colnum]))
            lex_token.push_back(line.data()[lex_colnum++]);
        return { NUM, lex_token.data(), "NUM", stoi(lex_token) };
    }
    else if (line.data()[lex_colnum] == '>' || line.data()[lex_colnum] == '<' ||
        line.data()[lex_colnum] == '=' || line.data()[lex_colnum] == '!') {
        if (line.data()[lex_colnum + 1] == '=') {
            lex_token.push_back(line.data()[lex_colnum++]);
            lex_token.push_back(line.data()[lex_colnum++]);
            return reserver();
        }
        else if (line.data()[lex_colnum] == '!') {
            lex_errors.push_back({ "NEQ","!=", lex_rownum, lex_colnum, line, "! must be followed by = " });
            lex_colnum++;
            return { NOTYP, "!", "NEQ" };
        }
        else {
            // single relation op or assign op
            lex_token.push_back(line.data()[lex_colnum++]);
            return reserver();
        }
    }
    else if (line.data()[lex_colnum] == '\'') {
        lex_colnum++;
        if (lex_colnum == line.size()) {
            lex_errors.push_back({ "CH", "", lex_rownum, lex_colnum, line, "Not a valid char" });
            return {};
        }
        if (isalnum(line.data()[lex_colnum]) || line.data()[lex_colnum] == '+' ||
            line.data()[lex_colnum] == '-' || line.data()[lex_colnum] == '*' ||
            line.data()[lex_colnum] == '/' || line.data()[lex_colnum] == '_') {
            lex_token.push_back(line.data()[lex_colnum++]);
            if (line.data()[lex_colnum] == '\'') {
                lex_colnum++;
                return { CH, lex_token.data(), "CH" };
            }
            else if (line.data()[lex_colnum] == '\"') {
                lex_colnum++;
                lex_errors.push_back({ "CH", lex_token.data(), lex_rownum, lex_colnum, line, "Wrong Quote" });
                return { NOTYP, lex_token.data(), "CH" };
            }// if its not ' or " then continue to identify str
        }
        else if (line.data()[lex_colnum] == '\'' || line.data()[lex_colnum] == '\"') {
            // Empty ch
            lex_colnum++;
            lex_errors.push_back({ "STR", "", lex_rownum, lex_colnum, line, "Empty STR" });
            return {};
        }
        else {
            lex_token.push_back(line.data()[lex_colnum++]);
            if (line.data()[lex_colnum] == '\'' || line.data()[lex_colnum] == '\"') {
                lex_colnum++;
                lex_errors.push_back({ "CH", lex_token.data(), lex_rownum, lex_colnum, line, "Not a valid char" });
                return { NOTYP, lex_token.data(), "CH" };
            }// if not ' or " then continue to identify str with wrong left single quote
        }

        int first_noalnum_position = -1;
        string temp_trunct;
        while (lex_colnum != line.size() && line.data()[lex_colnum] != '\"') {
            string s(1, line.data()[lex_colnum]);
            if (first_noalnum_position == -1 && lex_conversion.find(s) != lex_conversion.end()) {
                first_noalnum_position = lex_colnum; // store the trunct position
                temp_trunct = lex_token; // TODO:is this a quote or value
            }
            lex_token.push_back(line.data()[lex_colnum++]);
        }
        // Haven't found " and no trunct found, return to stored trunct point
        if (lex_colnum == line.size()) {
            if (first_noalnum_position != -1) {
                lex_colnum = first_noalnum_position - 1; // there is a ++ op later
                lex_token = temp_trunct;
            }
        }
        else {//col != size which means identified a " so move on
            lex_colnum++;
        }
        lex_errors.push_back({ "STR", lex_token.data(), lex_rownum, lex_colnum, line, "Wrong Quote Str" });
        return { NOTYP, lex_token.data(), "STR" };
    }
    else if (line.data()[lex_colnum] == '\"') {
        lex_colnum++; // Quote identified so move on
        if (lex_colnum == line.size()) {
            lex_errors.push_back({ "STR", "", lex_rownum, lex_colnum, line, "Empty String with no right quote" });
            return { NOTYP };
        }
        while (lex_colnum != line.size() && line.data()[lex_colnum] != '\"') {
            lex_token.push_back(line.data()[lex_colnum++]);
        }
        if (lex_colnum == line.size()) {
            lex_errors.push_back({ "STR", lex_token.data(), lex_rownum, lex_colnum, line, "Right quote not found" });
            return { NOTYP, lex_token.data(), "STR" };
        }
        lex_colnum++; // assume Quote identified so move on
        return { STR, lex_token.data(), "STR" };
    }
    else {
        // Identify all the rest like +-*/;:,(){}[] and some invalid ch
        lex_token.push_back(line.data()[lex_colnum++]);
        if (lex_conversion.find(lex_token.data()) == lex_conversion.end()) {
            lex_errors.push_back({ "CH", lex_token.data(), lex_rownum, lex_colnum, line, "Invalid Char" });
            return { NOTYP, lex_token.data(), "CH" };
        }
        return lex_conversion.find(lex_token.data())->second;
    }
}

Enum2str reserver() {
    Enum2str temp = { IDSY, lex_token.data(), "IDSY" };
    if (lex_conversion.find(lex_token.data()) != lex_conversion.end())
        temp = lex_conversion.find(lex_token.data())->second;
    // leave for grammer analyze to decide whether it is a function or var/const
    // return IDSY as default temporary result 
    return temp;
}

#endif // Syntactic

