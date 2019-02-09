#include "LexicalAnalyze.h"

using namespace std;

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

unordered_map<char, int> special_str_ch = {
    {'#',35}, {'$',36}, {'%',37}, {'&',38}, {'.',46}, 
    {'?',63}, {'@',64}, {'\\',92}, {'^',94}, {'|',124}, {'~',126}
};



//int main() {
//    string filename, line;
//    ifstream infile;
//    cout << "Please enter the route of souce file(Press <Enter> for default test code):" << endl;
//    getline(cin, filename);
//    if (filename == "") {
//        filename = "15061200_test.txt";
//        cout << "Using default 15061200_test.txt" << endl;
//    }
//    infile.open(filename);
//    if (!infile.is_open()) {
//        cout << "File " << filename << " not found." << endl;
//        cin.clear();
//        cin.sync();
//        cin.get();
//        exit(EXIT_FAILURE);
//    }
//    while (getline(infile, line))
//        lex_file.push_back(line);
//    infile.close();
//    cout << "Source file loaded." << endl;
//    cout << "Count     Row(1)  Col(1) " << endl;
//    // Save lines to vector<string>, iterate vector, for each line iterate ch inside
//    auto lineptr = lex_file.cbegin();
//    while (lineptr != lex_file.cend()) {
//        lex_output = lexicalanalyze(lineptr);
//        if (lex_output.sym == NOTYP) {
//            cout << "Cannot identify! Tried automatic correction..." << endl;
//        }
//        else if (lex_output.sym == ELINE or lex_output.sym == ENDF) continue;
//        lex_tokencount++;
//        // if tempoutput is NUM then integer will be stored in lex_output.num
//        if (lex_output.strsym.data() == "CH")
//            printf("%-10d %-5d %-5d %-10s \'%s\'\n", lex_tokencount, lex_rownum, lex_colnum - (int)lex_output.key.size() + 1, lex_output.strsym.data(), lex_output.key.data());
//        else if (lex_output.strsym.data() == "STR")
//            printf("%-10d %-5d %-5d %-10s \"%s\"\n", lex_tokencount, lex_rownum, lex_colnum - (int)lex_output.key.size() + 1, lex_output.strsym.data(), lex_output.key.data());
//        else
//            printf("%-10d %-5d %-5d %-10s %s\n", lex_tokencount, lex_rownum, lex_colnum - (int)lex_output.key.size() + 1, lex_output.strsym.data(), lex_output.key.data());
//    }
//    printError(lex_errors);
//    cin.clear();
//    cin.sync();
//    cout << "Press any key to continue¡­";
//    cin.get();
//    return 0;
//}

Enum2str lexicalanalyze(vector<string>::const_iterator& iter_line) {
    //cout << (*iter_line).size() << endl;
    while (lex_colnum >= (*iter_line).size()) {
        iter_line++;
        lex_rownum++;
        lex_colnum = 0;
        if (iter_line == lex_file.cend()) {
            return {ENDF};
        }
    }
    string line = *iter_line;
    lex_token.clear();
    while (isspace(line.data()[lex_colnum])) {
        lex_colnum++;
        if (lex_colnum == line.size())
            return {ELINE}; // multi line code is not allowed
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
        return { NUM, lex_token.data(), "NUM", stoi(lex_token)};
    }
    else if (line.data()[lex_colnum] == '>' || line.data()[lex_colnum] == '<' ||
        line.data()[lex_colnum] == '=' || line.data()[lex_colnum] == '!') {
        if (line.data()[lex_colnum + 1] == '=') {
            lex_token.push_back(line.data()[lex_colnum++]);
            lex_token.push_back(line.data()[lex_colnum++]);
            return reserver();
        }
        else if (line.data()[lex_colnum] == '!') {
            lex_errors.push_back({"NEQ","!=", lex_rownum, lex_colnum, line, "! must be followed by = "});
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
                return {CH, lex_token.data(), "CH"};
            }
            else if (line.data()[lex_colnum] == '\"') {
                lex_colnum++;
                lex_errors.push_back({"CH", lex_token.data(), lex_rownum, lex_colnum, line, "Wrong Quote"});
                return {NOTYP, lex_token.data(), "CH"};
            }// if its not ' or " then continue to identify str
        }
        else if (line.data()[lex_colnum] == '\'' || line.data()[lex_colnum] == '\"') {
            // Empty ch
            lex_colnum++;
            lex_errors.push_back({ "CH", "", lex_rownum, lex_colnum, line, "Empty char" });
            return {};
        }
        else {
            lex_token.push_back(line.data()[lex_colnum++]);
            if (line.data()[lex_colnum] == '\'' || line.data()[lex_colnum] == '\"') {
                lex_colnum++;
                lex_errors.push_back({"CH", lex_token.data(), lex_rownum, lex_colnum, line, "Not a valid char"});
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
            lex_errors.push_back({"STR", lex_token.data(), lex_rownum, lex_colnum, line, "Right quote not found"});
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
    // return IDSY as default temporary result, 
    return temp;
}

void printError(vector<Errormsg> errors) {
    if (errors.size() == 0) {
        cout << "0 errors, 0 warnings, mission complete !" << endl;
        return;
    }
    else {
        int errorcount = 1;
        for (auto errorptr = errors.cbegin(); errorptr != errors.cend(); ++errorptr) {
            printf("%d  (%d,%d) Intepret %s as %s\nCode: %s\n", errorcount++, errorptr->row,
                errorptr->col, errorptr->tmptoken.data(), errorptr->tmpsym.data(), errorptr->line.data());
            printf("MSG: %s\n", errorptr->tempmsg.data());
        }
    }
}