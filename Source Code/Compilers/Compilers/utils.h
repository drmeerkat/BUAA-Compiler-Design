#pragma once
#ifndef Utils
#define Utils
#include "SyntacticAnalyze.h"
string strreplace(const string&, const char&, const string&);
vector<string> split(const string&, const char&);
void printSymtab();
void printSyntacResult(string syntac, int count, bool iserror, string syntac_prefix);
void printError(vector<Errormsg>, vector<Errormsg2>);
void printQuat(string), printMips(string);
bool isNumCh(string, string&);

string strreplace(const string& input, const char& regex, const string& rep) {
    // split input by regex and rejoin using rep
    string output = "";
    for (auto iter = input.cbegin(); iter != input.cend(); iter++) {
        if (*iter == regex) {
            output += rep;
        }
        else {
            output.push_back(*iter);
        }
    }
    return output;
}

vector<string> split(const string& input, const char& regex) {
    string tmp = "";
    vector<string> output;
    for (auto iter = input.cbegin(); iter != input.cend(); iter++) {
        if (*iter == regex) {
            output.push_back(tmp);
            tmp.clear();
        }
        else {
            tmp.push_back(*iter);
            if (iter + 1 == input.cend()) {
                output.push_back(tmp);
            }
        }
    }

    return output;
}

void printError(vector<Errormsg> errors, vector<Errormsg2> errors2) {
    if (errors.size() == 0 && errors2.size() == 0) {
        cout << "0 errors, mission complete !" << endl;
        return;
    }
    else {
        cout << endl;
        cout << "########Errors Record########" << endl;
        int errorcount = 1;
        for (auto errorptr = errors.cbegin(); errorptr != errors.cend(); ++errorptr) {
            printf("%d  (%d,%d) Intepret %s as %s\nCode: %s\n", errorcount++, errorptr->row, errorptr->col, errorptr->tmptoken.data(), errorptr->tmpsym.data(), errorptr->line.data());
            printf("MSG : %s\n\n", errorptr->tempmsg.data());
        }
        for (auto errorptr = errors2.cbegin(); errorptr != errors2.cend(); ++errorptr) {
            printf("%d Row %d Syntactic/Semantic error\nCode: %s\n", errorcount++, errorptr->row, lex_file[errorptr->row - 1].data());
            printf("MSG : %s\n\n", errorptr->details.data());
        }
        cout << errorcount - 1 << " errors, mission failed !\n" << endl;
    }
}

void printSymtab() {
    // symbol tab <level name, level tab <var name, var_info>>
    cout << "=========================Symbol Table=========================" << endl;
    for (auto iter = syntac_symtab.cbegin(); iter != syntac_symtab.cend(); iter++) {
        cout << "\n==> Level Name: " << iter->first << endl;
        cout << "    Name    Kind    Type    Offset  (varcnt includes array elements but no const)" << endl;
        for (auto iter2 = iter->second.cbegin(); iter2 != iter->second.cend(); iter2++) {
            cout << "    " << iter2->first << "  ";
            cout << map_kind[iter2->second.kind] << "  " << map_type[iter2->second.type];
            cout << "  " << iter2->second.offset << endl;
            if (iter2->second.kind == 1) {//array
                cout << "      |__arrayinfo " << "Type: " << map_type[iter2->second.arrinfo.type];
                cout << "  Max_capacity: " << iter2->second.arrinfo.max_capacity << endl;
            }
            else if (iter2->second.kind == 2) {//func
                cout << "      |__funcinfo  " << "Funcname: " << iter2->second.funcinfo.func_name;
                cout << "  isret: " << iter2->second.funcinfo.isreturn;
                cout << "  retype: " << iter2->second.funcinfo.re_type;
                cout << "  varcnt: " << iter2->second.funcinfo.varcnt << endl;
                if (iter2->second.funcinfo.paralist.size() > 0) {
                    cout << "             |__paralist: ";
                    cout << map_type[iter2->second.funcinfo.paralist.front().first] << " " << iter2->second.funcinfo.paralist.front().second;
                    if (iter2->second.funcinfo.paralist.size() > 1) {
                        for (auto var = iter2->second.funcinfo.paralist.cbegin() + 1;
                            var != iter2->second.funcinfo.paralist.cend(); var++) {
                            cout << ", " << map_type[var->first] << " " << var->second;
                        }
                    }
                    cout << endl;
                }
            }
            else if (iter2->second.kind == 3) {//const
                cout << "      |__const value ";
                if (iter2->second.type == 10) cout << iter2->second.num << endl;
                if (iter2->second.type == 11) cout << "'" << iter2->second.ch << "'" << endl;
            }
            if (iter2->second.var_name != iter2->first) {
                cout << "Two var name not compatible" << iter2->second.var_name << endl;
            }
            if (iter2->second.level_name != iter->first) {
                cout << "Two level name not compatible" << iter2->second.level_name << endl;
            }

        }
    }
}

Quat formalizeQuat(Quat tmp) {
    if (tmp.op1.size() != 0 && split(tmp.op1, ' ').size() == 2) {
        tmp.op1 = split(tmp.op1, ' ').front();
    }
    if (tmp.op3.size() != 0 && split(tmp.op3, ' ').size() == 2) {
        tmp.op3 = split(tmp.op3, ' ').front();
    }
    if (split(tmp.op, ' ').front() == "Printf") return tmp;//op2 is str
    if (tmp.op2.size() != 0 && split(tmp.op2, ' ').size() == 2) {
        tmp.op2 = split(tmp.op2, ' ').front();
    }
    return tmp;
}

void printQuat(string subfix) {
    fstream fout("syntac_output_" + subfix + ".txt", ios::out);
    Quat tmp;
    vector<string> segment;
    int count = 0;
    if (!fout) {
        cout << "Cannot open syntac_output_" + subfix + ".txt" << endl;
        return;
    }
    for (auto quat = syntac_quat.cbegin(); quat != syntac_quat.cend(); quat++) {
        tmp = formalizeQuat(*quat);
        segment = split(tmp.op, ' ');
        fout << std::left << setw(5) << count++;
        if (segment.front() == "const") {
            fout << tmp.op << " " << tmp.op1 << " " << tmp.op2 << endl;
        }
        else if (segment.size() == 2 && segment.back() == "var") {
            fout << segment.back() << " " << segment.front() << " " << tmp.op1 << endl;
        }
        else if (segment.size() == 2 && segment.back() == "array") {
            fout << segment.back() << " " << segment.front() << " " << tmp.op1;
            fout << "[" << tmp.op2 << "]" << endl;
        }
        else if (segment.size() == 3 && segment[1] == "func") {
            fout << segment.front() << " " << segment.back() << "()" << endl;
        }
        else if (segment.size() == 2 && (segment.back() == "start" || segment.back() == "end")) {
            fout << tmp.op1 << " " << tmp.op << endl;
        }
        else if (segment.size() == 2 && segment.back() == "parameter") {
            fout << "para " << segment.front() << " " << tmp.op1 << endl;
        }
        else if (segment.size() == 4 && segment.front() == "call") {
            fout << "call " << tmp.op1 << endl;
            fout << std::left << setw(5) << count - 1 << tmp.op2 << "= RET" << endl;
        }
        else if (segment.size() == 2 && segment.front() == "push") {
            fout << "push " << tmp.op2 << endl;
        }
        else if (segment.size() == 1 && expop.find(segment.front()) != expop.end()) {
            fout << tmp.op1 << " = " << tmp.op2 << " " << tmp.op << " " << tmp.op3 << endl;
        }
        else if (segment.size() == 1 && segment.front() == "=[]") {
            fout << tmp.op1 << " = " << tmp.op2 << "[" << tmp.op3 << "]" << endl;
        }
        else if (segment.size() == 1 && segment.front() == "[]=") {
            fout << tmp.op1 << "[" << tmp.op2 << "] = " << tmp.op3 << endl;
        }
        else if (segment.size() == 1 && segment.front() == "=") {
            fout << tmp.op1 << " = " << tmp.op2 << endl;
        }
        else if (segment.size() == 1 && boolop.find(segment.front()) != boolop.end()) {
            string code;
            if (tmp.op == "==")
                code = "bne";
            else if (tmp.op == "<")
                code = "bge";
            else if (tmp.op == ">")
                code = "ble";
            else if (tmp.op == "<=")
                code = "bgt";
            else if (tmp.op == ">=")
                code = "blt";
            else if (tmp.op == "!=")
                code = "beq";
            else
                fout << "printQuat ERROR: no boolop exists" << endl; // this shouldn't happen
            fout << code << " " << tmp.op2 << " " << tmp.op3 << " " << tmp.op1 << endl;
        }
        else if (segment.size() == 1 && segment.front() == "switch") {
            fout << "switch (" << tmp.op1 << ")" << endl;
        }
        else if (segment.size() == 1 && segment.front().back() == ':') {
            fout << tmp.op << endl;
        }
        else if (segment.size() == 1 && segment.front() == "GOTO") {
            fout << "GOTO " + tmp.op1 << endl;
        }
        else if (segment.size() == 1 && segment.front() == "Return") {
            fout << "ret " << tmp.op1 << endl;
        }
        else if (segment.size() == 1 && segment.front() == "Printf") {
            fout << "printf \"" + tmp.op2 + "\", " + tmp.op3 << endl;
        }
        else if (segment.size() == 1 && segment.front() == "Scanf") {
            fout << "scanf " + tmp.op1 << endl;
        }
        else {
            cout << "printQuat ERROR: Unknown OP and Quatary !" << endl;
            cout << "Quat NO." << quat - syntac_quat.cbegin() << endl;
            return;
        }
    }
    cout << "Quat output is saved in ./syntac_output_" + subfix + ".txt" << endl;
}

void printMips(string subfix) {
    fstream fout("output_" + subfix + ".asm", ios::out);
    if (!fout) {
        cout << "Cannot open output_" + subfix + ".asm" << endl;
        return;
    }
    for (auto iter = syntac_mips.cbegin(); iter != syntac_mips.cend(); iter++) {
        if ((*iter).substr(0, 6) == "Label_" || (*iter).substr(0, 5) == "Func_" ||
            (*iter).substr(0, 5) == ".data" || (*iter).substr(0, 5) == ".text") {
            fout << setw(4) << "" << *iter << endl;
            continue;
        }
        fout << setw(8) << "" << *iter << endl;
    }
    fout.close();
    cout << endl;
    cout << "Trying to generate Mips..." << endl;
    cout << "Mips output is saved in ./output_" + subfix + ".asm" << endl;
}

bool isNumCh(string tmpname, string& result) {
    result = tmpname;
    if (tmpname.size() == 1 || tmpname.front() == '+' || tmpname.front() == '-' || isdigit(tmpname.front())
        || (tmpname.size() == 3 && tmpname.data()[0] == '\'')) {
        //ch or num(including const substituted in expression() load only) 'c' c are both char
        if ((tmpname.size() == 1 && !isdigit(tmpname.front())) || (tmpname.size() == 3 && tmpname.data()[0] == '\'')) {
            if (tmpname.size() == 1) {
                result = to_string(int(tmpname.front()));// change ch to a num
            }
            else {
                result = to_string(int(tmpname.data()[1]));// change ch to a num
            }
        }
        return true;
    }
    return false;
}


#endif // Utils
