#pragma once
#ifndef GenMips_opt
#define GenMips_opt
#include "SyntacticAnalyze.h"
#include "utils.h"
#include "myconst.h"

void genMips() {
    auto quatiter = syntac_quat.cbegin();

    syntac_mips.push_back(".data");
    for (auto iter = syntac_symtab["#global"].cbegin() + 1;
        iter != syntac_symtab["#global"].cend(); iter++) {
        // <levelname, vector<pair<varname, tabinfo>>>
        // every level start with its own func info
        if (iter->first != quatiter->op1 || quatiter->level_n != "#global") {
            break;
        }
        vector<string> tmp = split(quatiter->op, ' ');
        if (tmp.front() == "const") {
            genMips_const(quatiter, true);
        }
        else if (tmp.back() == "var") {
            genMips_var(quatiter, true);
        }
        else if (tmp.back() == "array") {
            genMips_array(quatiter, true);
        }
        else {
            cout << "Unknown Quat type" << endl;;
        }
    }
    unordered_map<string, int> count_strconst;
    int strlabel_cnt = 0;
    count_strconst[""]++;
    syntac_mips.push_back("str_n:.asciiz \"" + Newline + "\"");
    syntac_mips.push_back("str_" + to_string(strlabel_cnt) + ":.asciiz " + "\"" + "\"");
    syntac_strlabeltab[""] = "str_" + to_string(strlabel_cnt++);
    for (auto iter = syntac_quat.cbegin(); iter != syntac_quat.cend(); iter++) {
        if (iter->op == "Printf" && count_strconst[iter->op2] == 0) {
            count_strconst[iter->op2]++;
            string labelstr = to_string(strlabel_cnt++);
            syntac_mips.push_back("str_" + labelstr + ":.asciiz " + "\"" + iter->op2 + "\"");
            syntac_strlabeltab[iter->op2] = "str_" + labelstr; // str_1 : "somestr"
        }
    }

    syntac_mips.push_back(".text");
    syntac_mips.push_back("addi $sp, $sp, 4");//init sp to the top of the stack
    syntac_mips.push_back("move $fp, $sp");//init fp always point to the bottom of current AR
    syntac_mips.push_back("j Func_main");
    while (quatiter != syntac_quat.cend()) {
        auto tmp = split(quatiter->op, ' ');
        if (tmp.front() == "const") {
            genMips_const(quatiter, false);
        }
        else if (tmp.size() == 2 && tmp.back() == "var") {
            genMips_var(quatiter, false);
        }
        else if (tmp.size() == 2 && tmp.back() == "array") {
            genMips_array(quatiter, false);
        }
        else if (tmp.size() == 3 && tmp[1] == "func") {
            genMips_label(quatiter, tmp.back());//suffix is funcname
        }
        else if (tmp.size() == 2 && tmp.back() == "start") {
            genMips_funcdec(quatiter); // push para and save frame
        }
        else if (tmp.size() == 2 && tmp.back() == "end") {
            quatiter++; // already checked in genQuat() if no ret
        }
        else if (tmp.size() == 2 && tmp.back() == "parameter") {
            quatiter++; // do nothing in func declaration, push will handle them
        }
        else if (tmp.size() == 4 && tmp.front() == "call") {
            genMips_call(quatiter); // pass para and save tmpreg/ra
        }
        else if (tmp.size() == 2 && tmp.front() == "push") {
            genMips_push(quatiter);
        }
        else if (tmp.size() == 1 && expop.find(tmp.front()) != expop.end()) {
            genMips_exp(quatiter);
        }
        else if (tmp.size() == 1 && tmp.front() == "=[]") {
            genMips_arraycall(quatiter);
        }
        else if (tmp.size() == 1 && (tmp.front() == "[]=" || tmp.front() == "=")) {
            genMips_assign(quatiter);
        }
        else if (tmp.size() == 1 && boolop.find(tmp.front()) != boolop.end()) {
            genMips_bool(quatiter);//"==","<",">","!=","<=",">="
        }
        else if (tmp.size() == 1 && tmp.front() == "switch") {
            genMips_switch(quatiter);
        }
        else if (tmp.size() == 1 && tmp.front().back() == ':') {
            genMips_label(quatiter, "");
        }
        else if (tmp.size() == 1 && tmp.front() == "GOTO") {
            genMips_jump(quatiter);
        }
        else if (tmp.size() == 1 && tmp.front() == "Return") {
            genMips_ret(quatiter);
        }
        else if (tmp.size() == 1 && tmp.front() == "Printf") {
            genMips_printf(quatiter);
        }
        else if (tmp.size() == 1 && tmp.front() == "Scanf") {
            genMips_scanf(quatiter);
        }
        else {
            cout << "ERROR: Unknown OP and Quatary !" << endl;
            cout << "Quat NO." << quatiter - syntac_quat.cbegin() << endl;
            return;
        }
    }

    return;
}
//Every time you call something start with genMips_xxx() 
//it will move the quat iter to the next quat to be interpreted
void genMips_const(vector<Quat>::const_iterator& qiter, bool isglob) {
    qiter++;
    return;
}

void genMips_var(vector<Quat>::const_iterator& qiter, bool isglob) {
    Quat tmp = *qiter;
    if (isglob) {
        syntac_mips.push_back("var_" + tmp.op1 + ": .space 4" + " #" + to_string(qiter - syntac_quat.cbegin()));
        qiter++;
        return;
    }
    qiter++;
    return;
}

void genMips_array(vector<Quat>::const_iterator& qiter, bool isglob) {
    Quat tmp = *qiter;
    if (isglob) {
        syntac_mips.push_back("arr_" + tmp.op1 + ": .space " + to_string(4 * stoi(tmp.op2)) + " #" + to_string(qiter - syntac_quat.cbegin()));
        qiter++;
        return;
    }
    qiter++;
    return;
}

void genMips_label(vector<Quat>::const_iterator& qiter, string suffix) {
    Quat tmp = *qiter;
    int position;
    if (suffix != "") {
        syntac_mips.push_back("Func_" + suffix + ":" + " #" + to_string(qiter - syntac_quat.cbegin()));//#Func_xxx:
        string funcname = split(tmp.op, ' ').back();
        if (lookuptab(tmp.tabop1, funcname, funcname, position, {}, { strmap_kind["func"] }) == false) {
            cout << "ERROR: no func declared" << endl; // this shouldn't happen
        }
    }
    else {
        syntac_mips.push_back(tmp.op + " #" + to_string(qiter - syntac_quat.cbegin()));//Label_num:
    }
    qiter++;
    return;
}

string allocateTempReg(vector<Quat>::const_iterator qiter) {
    static int num = 0; //0-4
    int currentnum = num;
    if (num > 4) num = 0;
    // if all in use return a symbol so that func could save and use
    while (gen_regs.find("$t" + to_string(num)) != gen_regs.end()) {//if not found in genregs then just allocate
        Reginfo reg = gen_regs.find("$t" + to_string(num))->second;//this treg has been used
        if (reg.var_name.front() == '#') {//this treg stored a tmpvar
            if (gen_multiuse.find(reg.var_name) == gen_multiuse.end()) {
                cout << "ERROR: Unknown tmpvar name stored in tmp reg" << endl;
            }
            if (gen_multiuse[reg.var_name].back() < (int)(qiter - syntac_quat.cbegin())) {
                break;
            }// if last time used line NO is smaller than current line then it can be override
        }
        num++;
        if (currentnum == num) cout << "ERROR: loop back, tregs are full" << endl;
        //currently this shouldn't happen for only tmpvar is allocated to treg
        if (num > 4) num = 0;
    }
    return ("$t" + to_string(num++));
}

string allocateGlobReg(vector<Quat>::const_iterator qiter) {
    static int num = 0; //0-2
    int currentnum = num, iter = num;
    if (num > 2) num = 0;
    while (gen_regs.find("$s" + to_string(num)) != gen_regs.end()) {//if not found in genregs then just allocate
        Reginfo reg = gen_regs.find("$s" + to_string(num))->second;//this treg has been used
        if (reg.isdirty == false) {//this sreg is clear
            break;
        }
        else if (reg.var_name.front() == '#') {//this reg hold a no use tmpvar
            if (gen_multiuse[reg.var_name].back() < (int)(qiter - syntac_quat.cbegin())) {
                break;
            }
        }
        num++;
        if (num == currentnum) {//loop around back -> both fp and sreg is full
            while (gen_regs.find("$s" + to_string(iter))->second.var_name.front() == '#') {
                iter++;
                if (iter > 2) iter = 0;
                if (iter == currentnum)//loop around back
                    cout << "ERROR: all sreg and fp are filled with tmpvar" << endl;//shouldn't happen
            }
            gen_regs["$s" + to_string(iter)].needwb = true;//this sreg doesn't store a tmpvar
            Reginfo savetmp = gen_regs["$s" + to_string(iter)];
            //needwb then findreg will return false, thus findvar will save it back to stack
            genMips_findvar(savetmp.var_name + " " + savetmp.level_name, savetmp.regname, false, qiter, savetmp.subscript);
            num = iter + 1;//point to next
            return ("$s" + to_string(iter));
        }
        if (num > 2) num = 0;
    }
    if (find(gen_usedsreg.cbegin(), gen_usedsreg.cend(), num) == gen_usedsreg.cend()) {
        gen_usedsreg.push_back(num);//save sreg for caller
        syntac_mips.push_back("sw $s" + to_string(num) + ", " + to_string(4 * 10 + 4 * num) + "($fp) #" + to_string(qiter - syntac_quat.cbegin()) + "save for caller");
    }
    return ("$s" + to_string(num++));
}
//specialtag
//{"funcname", {"var1","var2","","","","","","","",""}} max var10 treg5-9, sreg3-7
const unordered_map<string, vector<string>> func_dict = { {"complete_num", {"i","j","n","s","x1","m","h","x2","leap","a"}} };
const vector<string> opt_regs = { "$s3", "$s4", "$s5", "$s6", "$s7", "$t5", "$t6", "$t7", "$t8", "$t9" };
//when return need to restore sreg / save sreg / these tregs also need to save/restore
vector<string> saved_optregs = {};


void genMips_recordreg(string reg, string varname, string levelname, string subscript = "", bool clear = false) {//only called by findvar
    Reginfo tmpinfo;
    if (subscript != "") return;//all array value directly use stack
    if (reg.data()[1] != 't'
        && reg != "$a1" && reg != "$a2" && reg != "$a3" && find(opt_regs.cbegin(), opt_regs.cend(), reg) == opt_regs.cend()) return;//special reg like fp/v/g/sp/a0 no sreg allocation
    //(reg.data()[1] != 's' || reg.data()[2] == 'p') &&
    if (gen_regs.find(reg) == gen_regs.end()) {//it's first time use reg
        tmpinfo.regname = reg;
    }
    else if (gen_regs.find(reg)->second.isdirty && !clear && reg.data()[1] != 't') {
        return;
    }
    if (clear) {//currently actually only sreg and fp need dirty bit
        if (reg.data()[1] == 's') {//clear sreg
            if (gen_regs.find(reg)->second.needwb == true) {
                //only when write back the sreg will save to the corresponding place
                gen_regs.find(reg)->second.isdirty = false;
            }
        }//cause link treg will also use treg and it will allocate free tmpreg automatically
        //so don't need to set clear to treg when link or save 
        return;
    }
    if (gen_regs.find(reg) == gen_regs.end()) {//first time use
        gen_regs[reg] = tmpinfo;
    }
    gen_regs[reg].var_name = varname;
    gen_regs[reg].isdirty = true;
    gen_regs[reg].subscript = subscript;
    gen_regs[reg].needwb = false;
    gen_regs[reg].level_name = levelname;
}

bool genMips_findreg(Reginfo& findresult, string levelname, string varname, string subscript) {
    Reginfo tmp;
    for (auto iter = gen_regs.cbegin(); iter != gen_regs.cend(); iter++) {
        tmp = iter->second;
        if (tmp.level_name == levelname && tmp.var_name == varname && tmp.subscript == subscript) {
            if (tmp.needwb) {
                return false;
            }
            findresult = tmp;
            return true;
        }
    }
    return false;
}

void genMips_findvar(string tmpname, string& reg, bool isload, vector<Quat>::const_iterator qiter, string subscript = "") {
    // is load then find tmpname in stack/regs and load/move tmpname into reg
    // is not load then find tmpname in stack/regs and save/move reg to tempname
    int offset = -1, position = -1;
    bool ismove = false;
    string tmpreg = "", levelname;
    vector<string> var = split(tmpname, ' ');
    Tabinfo findresult;
    Reginfo tmpreginfo;
    if (tmpname.size() == 1 || tmpname.front() == '+' || tmpname.front() == '-' || isdigit(tmpname.front())
        || (tmpname.size() == 3 && tmpname.data()[0] == '\'')) {
        //ch or num(including const substituted in expression()) load only 'c' c are both char
        if ((tmpname.size() == 1 && !isdigit(tmpname.front())) || (tmpname.size() == 3 && tmpname.data()[0] == '\'')) {
            if (tmpname.size() == 1) {
                tmpname = to_string(int(tmpname.front()));// change ch to a num
            }
            else {
                tmpname = to_string(int(tmpname.data()[1]));// change ch to a num
            }
        }
        syntac_mips.push_back("li " + reg + ", " + tmpname + " #" + to_string(qiter - syntac_quat.cbegin()));//when it's a const/value, assert isload true
        if (isload == false) cout << "ERROR: shouldn't write to a const or value" << endl;
        return;
    }
    else if (tmpname == "$0") {
        if (isload) {
            reg = "$0";
        }
        else {
            cout << "ERROR: cannot write to $0" << endl;
        }
        return;
    }
    else if (var.size() == 2 && subscript == "") {
        // simple var/para save/load
        // find the var on stack/regs symbol tab should look like this: funcname, paras, localvars
        if (lookuptab(findresult, var.back(), var.front(), position, {}, { 4,5 }) == false) {
            cout << "ERROR: not registered var/para" << endl;//this shouldn't happen
        }
        else if (findresult.kind != 4 && findresult.kind != 5) {
            cout << "ERROR: wrong kind with same name" << endl;
        }
        else if (genMips_findreg(tmpreginfo, findresult.level_name, var.front(), "")) {//find in regs
            ismove = true;
            tmpreg = tmpreginfo.regname;
        }
        else {
            if (position == -1) cout << "ERROR: position should be found in lookuptab" << endl;
            Tabinfo levelinfo = syntac_symtab[findresult.level_name].front().second;
            int total_cnt = levelinfo.funcinfo.varcnt + (int)(levelinfo.funcinfo.paralist.size()) + 19;
            if (findresult.level_name != "#global") {//local var/para
                switch (findresult.kind) {
                    //stack position: para ret vars sregs fp//position counts only para and vars
                case 4://var
                    offset = 4 * (total_cnt - position - 1);//ret
                    break;
                case 5://para
                    offset = 4 * (total_cnt - position);
                    break;
                default:
                    cout << "ERROR: wrong kind not var or para" << endl;
                }
                tmpreg = "$fp";
            }
            else if (findresult.level_name == "#global") {//global var
                offset = 0;
                tmpreg = "$gp";
                syntac_mips.push_back("la $gp, var_" + findresult.var_name + " #" + to_string(qiter - syntac_quat.cbegin()));
            }
            else {
                cout << "ERROR: wrong var level" << endl;//this shouldn't happen
            }
        }
        levelname = findresult.level_name;
    }
    else if (tmpname.front() == '#') {
        // tmpvar save / load only when find in regs
        pair<string, string> tmpvar;// <#No, levelname>
        for (auto iter = syntac_tmpvar.cbegin(); iter != syntac_tmpvar.cend(); iter++) {
            if (iter->first == var.front()) {
                tmpvar = *iter;
                break;
            }
        }
        offset = 0;
        if (genMips_findreg(tmpreginfo, tmpvar.second, tmpvar.first, "")) {
            ismove = true;//already have this tmpvar
            tmpreg = tmpreginfo.regname;//find in regs
        }
        else if (isload) {//not find it's a new tmpvar
            cout << "ERROR: tmpvar has not been calculated yet" << endl;//shouldn't happen
        }//else tmpreg="" and issave which means to link reg with tmpvar name
        levelname = tmpvar.second;
    }
    else if (var.size() == 2 && subscript != "") {
        // array save/load
        if (lookuptab(findresult, var.back(), var.front(), position, {}, { 1 }) == false) {
            cout << "ERROR: not registered array" << endl;//this shouldn't happen
        }
        else if (findresult.kind != 1) {
            cout << "ERROR: wrong kind with same array's name" << endl;
        }
        else if (genMips_findreg(tmpreginfo, findresult.level_name, var.front(), subscript)) {
            /*ismove = true;
            tmpreg = tmpreginfo.regname;*///currently all array write directly back to stack
            cout << "ERROR: no array is stored in regs" << endl;
        }
        else {
            if (position == -1) cout << "ERROR: position should be positive" << endl;
            Tabinfo levelinfo = syntac_symtab[findresult.level_name].front().second;
            int total_cnt = levelinfo.funcinfo.varcnt + (int)(levelinfo.funcinfo.paralist.size()) + 19;
            if (findresult.level_name != "#global") {//not global
                switch (findresult.kind) {
                    //stack position: para ret vars //position counts only para and vars
                case 1://array
                    if (!isdigit(subscript.front())) {//subscript is a name
                        genMips_findvar(subscript, tmpreg, true, qiter);
                        if (tmpreg == "")
                            cout << "ERROR: subscript is not calculated" << endl;
                        string treg = allocateTempReg(qiter), treg1 = allocateTempReg(qiter);
                        syntac_mips.push_back("move " + treg + ", " + tmpreg + " #" + to_string(qiter - syntac_quat.cbegin()) + "find subscript reg");//in case tmpreg is sreg
                        syntac_mips.push_back("li " + treg1 + ", " + to_string(total_cnt - position - 1));
                        syntac_mips.push_back("sub " + treg + ", " + treg1 + ", " + treg);
                        syntac_mips.push_back("mul " + treg + ", " + treg + ", 4");
                        syntac_mips.push_back("add " + treg + ", " + treg + ", $fp #" + to_string(qiter - syntac_quat.cbegin()) + "calculate subscript");//addr = fp+treg
                        offset = 0;
                        tmpreg = treg;//treg holds calculated offset
                    }
                    else {
                        offset = 4 * (total_cnt - position - 1 - stoi(subscript));//ret
                        tmpreg = "$fp";
                    }
                    break;
                default:
                    cout << "ERROR: wrong kind not array" << endl;
                    tmpreg = "$fp";
                }
            }
            else if (findresult.level_name == "#global") {//global array
                if (!isdigit(subscript.front())) {//subscript is a name
                    genMips_findvar(subscript, tmpreg, true, qiter);
                    if (tmpreg == "")
                        cout << "ERROR: subscript is not calculated" << endl;
                    string treg = allocateTempReg(qiter);
                    syntac_mips.push_back("move " + treg + ", " + tmpreg + " #find subscript reg");//in case tmpreg is sreg
                    syntac_mips.push_back("mul " + treg + ", " + treg + ", 4");
                    syntac_mips.push_back("la $gp, arr_" + findresult.var_name);
                    syntac_mips.push_back("add " + treg + ", " + treg + ",$gp #calculate subscript");//addr = gp+treg
                    offset = 0;
                    tmpreg = treg;//treg holds calculated addr
                }
                else {
                    offset = 4 * stoi(subscript);
                    tmpreg = "$gp";
                    syntac_mips.push_back("la $gp, arr_" + findresult.var_name + " #" + to_string(qiter - syntac_quat.cbegin()));
                }
            }
            else {
                cout << "ERROR: wrong array level" << endl;//this shouldn't happen
            }
        }
        levelname = findresult.level_name;
    }
    else {
        cout << "ERROR: Wrong tmpname in findvar " + tmpname << endl;//this shouldn't happen
    }
    //if (tmpreg == "") when link tmpvar to reg
    if (isload) {//from tmp to reg
        if (ismove) {//already in reg and tmpreg is not a special reg
            if (reg.data()[1] == 'a' || reg == "$v1") {//if reg is v1, a0-a3 then must move
                syntac_mips.push_back("move " + reg + ", " + tmpreg + " #" + to_string(qiter - syntac_quat.cbegin()) + " " + tmpname);
            }
            else {
                reg = tmpreg;
            }
        }
        else {//stack
            if (reg.data()[1] == 'a' || reg == "$v1") {//if reg is v1, a0-a3 then reg cannot change

            }
            else if (tmpname.front() != '#') {// not a tmpvar
                reg = allocateGlobReg(qiter);
            }
            syntac_mips.push_back("lw " + reg + ", " + to_string(offset) + "(" + tmpreg + ")" + " #" + to_string(qiter - syntac_quat.cbegin()) + " " + tmpname);
        }
        if (reg != tmpreg) {
            genMips_recordreg(reg, var.front(), levelname, subscript);//set dirty bit
        }
    }
    else {//from reg to tmp
        if (ismove) {
            genMips_recordreg(tmpreg, var.front(), levelname, subscript);//set dirty bit
            if (tmpreg.data()[1] == 't' && (tmpreg.data()[1]-'0') < 5) {//specialtag
                cout << "ERROR: old treg/tmpvar cannot be reused as left value" << endl;
            }
            syntac_mips.push_back("move " + tmpreg + ", " + reg + " #" + to_string(qiter - syntac_quat.cbegin()) + " " + tmpname);
        }
        else {
            if (tmpname.front() == '#') {//link tmpvar with treg
                if (reg.data()[1] == 't') {
                    genMips_recordreg(reg, tmpname, levelname, subscript);
                }
                else {//reg is a sreg or a special reg
                    string tmpreg = allocateTempReg(qiter);
                    syntac_mips.push_back("move " + tmpreg + ", " + reg + " #" + to_string(qiter - syntac_quat.cbegin()));
                    genMips_recordreg(tmpreg, tmpname, levelname, subscript);
                }
            }
            else {//save reg to var on stack and clear reg(if it is sreg/fp)
                genMips_recordreg(reg, var.front(), levelname, subscript, true);//clear dirty bit
                syntac_mips.push_back("sw " + reg + ", " + to_string(offset) + "(" + tmpreg + ")" + " #" + to_string(qiter - syntac_quat.cbegin()) + " " + tmpname);
            }
        }
    }
    return;
}

void genMips_funcdec(vector<Quat>::const_iterator& qiter) {
    Quat tmp = *qiter;
    // though the name is "declare" it indeed sets up AR on stack
    // when used, sregs will be reload back from child func's stack
    // para already in stack then ret, var/array, sregs/fp
    syntac_mips.push_back("addi $sp, $sp, -4 #" + to_string(qiter - syntac_quat.cbegin()));
    syntac_mips.push_back("sw $ra, 0($sp) #" + to_string(qiter - syntac_quat.cbegin())); // save ra
    Tabinfo findresult;
    int position;
    if (lookuptab(findresult, tmp.op1, tmp.op1, position, {}, { strmap_kind["func"] }) == false) {
        cout << "ERROR: Func not declared line" << endl;
    }// 4*varcnt+4*8sreg+4*10treg
    syntac_mips.push_back("addi $sp, $sp, -" + to_string(4 * findresult.funcinfo.varcnt + 4 * 18) + " #" + to_string(qiter - syntac_quat.cbegin()));
    syntac_mips.push_back("move $fp, $sp");//initialize fp
    gen_usedsreg.clear();//clear the record of sreg used. update in findvar()
    gen_pararecord.clear();//add records when call some other child func
    gen_regs.clear();//reginfo is localized to every single function s,t,fp
    Tabinfo tmptab = syntac_symtab[tmp.op1].front().second;
    int areg_cnt = 1;
    for (auto para = tmptab.funcinfo.paralist.cbegin(); para != tmptab.funcinfo.paralist.cend(); para++) {
        if (areg_cnt <= 3) {
            genMips_recordreg("$a" + to_string(areg_cnt), para->second, tmptab.level_name);
            areg_cnt++;
        }
    }//link a1-a3 with first three parameter(if has)
    //link s3-s7 and t5-t9 with given 10 paras
    if (func_dict.find(findresult.var_name) != func_dict.cend()) {
        vector<string> varlist = func_dict.find(findresult.var_name)->second;
        string tmpvar, tmpreg;
        if (varlist.size() > 10){ 
            cout << "Given more than 10 vars to put into regs" << endl;
        }
        for (auto iter = varlist.cbegin(); iter != varlist.cend(); iter++) {
            tmpreg = opt_regs[(int)(iter-varlist.cbegin())];
            tmpvar = *iter;
            genMips_recordreg(tmpreg, tmpvar, tmptab.level_name);
        }
    }
    qiter++;
    return;
}

void genMips_call(vector<Quat>::const_iterator& qiter) {
    Quat tmp = *qiter;
    string retvalue = "$v1", lastuser;//op2 is ret value's tmpname, op3 is caller name
    Reginfo tmpreg;
    vector<int> saved_tregs;
    int funcNO = stoi(split(tmp.op, ' ').back());
    int offset = 0;
    //save treg and special regs
    saved_optregs.clear();
    int nextfunc_vars = 0, saved_regs_cnt = 0;
    if (func_dict.find(tmp.op1) != func_dict.cend()) {
        nextfunc_vars = (int)func_dict.find(tmp.op1)->second.size();
    }
    for (auto iter = gen_regs.cbegin(); iter != gen_regs.cend(); iter++) {
        tmpreg = iter->second;
        if (tmpreg.regname.data()[1] == 't' && (tmpreg.regname.data()[2] - '0') < 5) {
            if (gen_multiuse[tmpreg.var_name].back() >= (int)(qiter - syntac_quat.cbegin())) {//the treg store a still in use tempvar
                saved_tregs.push_back(tmpreg.regname.data()[2] - '0');
                offset = saved_tregs.back() * 4;
                syntac_mips.push_back("sw $t" + to_string(saved_tregs.back()) + ", " + to_string(offset) + "($fp)" + " #" + to_string(qiter - syntac_quat.cbegin()));
            }
        }
        //specialtag
        else if (find(opt_regs.cbegin(), opt_regs.cend(), tmpreg.regname) != opt_regs.cend()) {
            if (nextfunc_vars > 0 && opt_regs[saved_regs_cnt] == tmpreg.regname) {
                saved_regs_cnt++;//move the pointer to next special reg
                nextfunc_vars--;//once saved then sub
                if (tmpreg.regname.data()[1] == 't') {
                    offset = (tmpreg.regname.data()[2] - '0') * 4;
                }
                else {//sreg
                    offset = (tmpreg.regname.data()[2] - '0') * 4 + 40;
                }
                saved_optregs.push_back(tmpreg.regname);
                syntac_mips.push_back("sw " + tmpreg.regname + ", " + to_string(offset) + "($fp)   #" + to_string(qiter - syntac_quat.cbegin()));
            }
        }
    }
    syntac_mips.push_back("jal Func_" + tmp.op1 + " #" + to_string(qiter - syntac_quat.cbegin()));
    //after return fp could be wrong due to the half pushed paras
    int total_size = 0;
    for (auto iter = gen_pararecord.cbegin(); iter != gen_pararecord.cend(); iter++) {
        if (iter == gen_pararecord.cend() - 1 && iter->first == funcNO) break;//callee itself not counted
        total_size += int(iter->second.size());
    }
    if (total_size > 0)
        syntac_mips.push_back("addi $fp, $fp, " + to_string(total_size * 4) + " #" + to_string(qiter - syntac_quat.cbegin()) + " half para/adjust fp");
    //retrieve areg using callee's pararecord
    if (gen_pararecord.size() > 0) {//at least has one child func with para has been called
        if (gen_pararecord.back().first == funcNO) {//in case a nonpara func is called
            for (auto iter = gen_pararecord.back().second.cbegin(); iter != gen_pararecord.back().second.cend(); iter++) {
                if (iter - gen_pararecord.back().second.cbegin() < 3) {//0,1,2
                    int aregnum = int(iter - gen_pararecord.back().second.cbegin()) + 1;//1,2,3
                    lastuser = genMips_lastareguser(aregnum, tmp.op3);
                    if (lastuser == tmp.op3) {//caller used this areg
                        Tabinfo tmptab = syntac_symtab[tmp.op3].front().second;
                        offset = -aregnum + tmptab.funcinfo.varcnt + (int)(tmptab.funcinfo.paralist.size()) + 19;
                        offset *= 4;
                        syntac_mips.push_back("lw $a" + to_string(aregnum) + ", " + to_string(offset) + "($fp)" + " #" + to_string(qiter - syntac_quat.cbegin()) + " load for caller");
                        gen_regs["$a" + to_string(aregnum)].needwb = false;
                    }
                    else if (lastuser != "") {//someone in the record used this areg
                        int offset = 0;
                        for (int i = stoi(lastuser); i < gen_pararecord.size(); i++) {//0,1,2
                            if (i == gen_pararecord.size() - 1) break;//callee itself not counted
                            offset += int(gen_pararecord[i].second.size());
                        }
                        offset -= aregnum;//currently sp point to the bottom of last callee
                        syntac_mips.push_back("lw $a" + to_string(aregnum) + ", " + to_string(4 * offset) + "($sp) #" + to_string(qiter - syntac_quat.cbegin()) + " load for last callee");
                    }
                    else {}// no one used this areg before
                }
            }
            //callee pararecord is useless now
            gen_pararecord.pop_back();
        }
    }
    //retrieve result (result has a name or it is a new tmpvar)
    genMips_findvar(tmp.op2, retvalue, false, qiter);
    //retrieve treg and special treg/sreg
    for (auto iter = saved_tregs.cbegin(); iter != saved_tregs.cend(); iter++) {
        offset = (*iter) * 4;
        syntac_mips.push_back("lw $t" + to_string(*iter) + ", " + to_string(offset) + "($fp)" + " #" + to_string(qiter - syntac_quat.cbegin()));
    }
    //specialtag
    for (auto iter = saved_optregs.cbegin(); iter != saved_optregs.cend(); iter++) {
        if ((*iter).data()[1] == 't') {
            offset = ((*iter).data()[2] - '0') * 4;
        }
        else {//sreg
            offset = ((*iter).data()[2] - '0') * 4 + 40;
        }
        syntac_mips.push_back("lw " + (*iter) + ", " + to_string(offset) + "($fp)" + " #" + to_string(qiter - syntac_quat.cbegin()));
    }
    qiter++;
    return;
}

void genMips_push(vector<Quat>::const_iterator& qiter) {
    Quat tmp = *qiter;//op1 para name op2 value op3 caller level name callee level name
    string lastuser, treg = allocateTempReg(qiter), caller = split(tmp.op3, ' ').front(), callee = split(tmp.op3, ' ').back();
    int funcNO = stoi(split(tmp.op3, ' ')[1]);
    vector<string> paras = {};
    genMips_findvar(tmp.op2, treg, true, qiter); // calculate result and load into treg
    syntac_mips.push_back("addi $sp, $sp, -4");//allocate para room
    if (gen_pararecord.size() == 0) {
        gen_pararecord.push_back(make_pair(funcNO, paras));// a new func's pararecord
    }
    else if (funcNO != gen_pararecord.back().first) {
        gen_pararecord.push_back(make_pair(funcNO, paras));// a new func's pararecord
    }
    gen_pararecord.back().second.push_back(tmp.op1);

    if (gen_pararecord.back().second.size() <= 3) {//current func para pushed cnt 1/2/3
        int aregnum = int(gen_pararecord.back().second.size());//1,2,3
        lastuser = genMips_lastareguser(aregnum, caller);
        if (lastuser == caller) {//caller used this areg
            Tabinfo tmptab = syntac_symtab[caller].front().second;
            int offset = -aregnum + tmptab.funcinfo.varcnt + (int)(tmptab.funcinfo.paralist.size()) + 19;
            syntac_mips.push_back("sw $a" + to_string(aregnum) + ", " + to_string(4 * offset) + "($fp)" + " #" + to_string(qiter - syntac_quat.cbegin()) + " push for caller");
            gen_regs["$a" + to_string(aregnum)].needwb = true;//thus other para will look for it on stack
        }
        else if (lastuser != "") {//someone in the record used this areg
            int offset = 0;
            for (int i = stoi(lastuser); i < gen_pararecord.size(); i++) {
                if (i == gen_pararecord.size() - 1) break;//callee itself not counted
                offset += int(gen_pararecord[i].second.size());
            }
            syntac_mips.push_back("sw $a" + to_string(aregnum) + ", " + to_string(4 * offset) + "($sp)" + " #" + to_string(qiter - syntac_quat.cbegin()) + " push for last callee");
        }
        else {}// no one used this areg before
        syntac_mips.push_back("move $a" + to_string(aregnum) + ", " + treg + " #" + to_string(qiter - syntac_quat.cbegin()));//now areg is clear
    }
    else {//more than 3 para
        syntac_mips.push_back("sw " + treg + ", 0($sp) #" + to_string(qiter - syntac_quat.cbegin()));
    }
    qiter++;
    return;
}

string genMips_lastareguser(int aregnum, string caller) {//assume that gen_pararecord.cend() - 1 = callee
    string lastuser = "";
    if (gen_regs.find("$a" + to_string(aregnum)) != gen_regs.cend()) {
        lastuser = caller;
    }
    for (auto iter = gen_pararecord.cbegin(); iter != gen_pararecord.cend(); iter++) {
        if (iter == gen_pararecord.cend() - 1) break;//without current callee itself
        if (iter->second.size() >= aregnum) {
            lastuser = to_string(iter - gen_pararecord.cbegin());//0,1,2,...
        }
    }
    return lastuser;
}

void genMips_exp(vector<Quat>::const_iterator& qiter) {
    Quat tmp = *qiter;
    string treg1 = allocateTempReg(qiter), treg2 = allocateTempReg(qiter), treg3 = allocateTempReg(qiter);
    string code, op2result, op3result;
    int result;

    switch (tmp.op.front()) {
    case '+':
        code = "addu"; break;
    case '-':
        code = "subu"; break;
    case '*':
        code = "mul"; break;//ignore the HI LO just using LO as result
    case '/':
        code = "div"; break;//ignore the HI LO just using LO as result
    default:
        cout << "ERROR: not exist expop" << endl;//shouldn't happen
    }
    if (isNumCh(tmp.op2, op2result) && isNumCh(tmp.op3, op3result)) {
        switch (tmp.op.front()) {
        case '+':
            result = stoi(op2result) + stoi(op3result); break;
        case '-':
            result = stoi(op2result) - stoi(op3result); break;
        case '*':
            result = stoi(op2result) * stoi(op3result); break;//ignore the HI LO just using LO as result
        case '/':
            result = stoi(op2result) / stoi(op3result); break;//ignore the HI LO just using LO as result
        default:
            cout << "ERROR: not exist expop" << endl;//shouldn't happen
        }
        syntac_mips.push_back("li "+treg1+", "+to_string(result)+ " #" + to_string(qiter - syntac_quat.cbegin()));
    }
    else {
        genMips_findvar(tmp.op2, treg2, true, qiter);
        genMips_findvar(tmp.op3, treg3, true, qiter);
        syntac_mips.push_back(code + " " + treg1 + ", " + treg2 + ", " + treg3 + " #" + to_string(qiter - syntac_quat.cbegin()));
    }
    genMips_findvar(tmp.op1, treg1, false, qiter);//op1 has a name or a new tmpvar
    qiter++;
    return;
}


void genMips_arraycall(vector<Quat>::const_iterator& qiter) {
    Quat tmp = *qiter;
    string arrvalue = allocateTempReg(qiter); // array address
    // TODO: the arraycall should be optimized and merged into exp or assign
    genMips_findvar(tmp.op2 + " " + tmp.tabop2.level_name, arrvalue, true, qiter, tmp.op3);// op1 = op2[op3]
    genMips_findvar(tmp.op1, arrvalue, false, qiter);//here op1 is a new tmpvar
    qiter++;
    return;
}

void genMips_assign(vector<Quat>::const_iterator& qiter) {
    Quat tmp = *qiter;
    string value = allocateTempReg(qiter);

    if (tmp.op == "[]=") {
        // op1[op2] = op3
        genMips_findvar(tmp.op3, value, true, qiter);
        genMips_findvar(tmp.op1 + " " + tmp.tabop1.level_name, value, false, qiter, tmp.op2);//array assign
    }
    else {
        // op1 = op2
        genMips_findvar(tmp.op2, value, true, qiter);
        genMips_findvar(tmp.op1, value, false, qiter);//here op1 must be a new tmpvar
    }
    qiter++;
    return;
}

void genMips_bool(vector<Quat>::const_iterator& qiter) {
    Quat tmp = *qiter;
    string treg3 = allocateTempReg(qiter), treg2 = allocateTempReg(qiter);
    string code;
    genMips_findvar(tmp.op2, treg2, true, qiter);
    genMips_findvar(tmp.op3, treg3, true, qiter);
    // jump to op1 if (op2 op op3) true or not true "==","<",">","!=","<=",">="
    if (tmp.op == "==") {
        code = "bne";
    }
    else if (tmp.op == "<") {
        code = "bge";
    }
    else if (tmp.op == ">") {
        code = "ble";
    }
    else if (tmp.op == "<=") {
        code = "bgt";
    }
    else if (tmp.op == ">=") {
        code = "blt";
    }
    else if (tmp.op == "!=") {
        code = "beq";
    }
    else {
        cout << "ERROR: no boolop exists" << endl; // this shouldn't happen
    }
    syntac_mips.push_back(code + " " + treg2 + ", " + treg3 + ", " + tmp.op1 + " #" + to_string(qiter - syntac_quat.cbegin()));
    qiter++;
    return;
}

void genMips_scanf(vector<Quat>::const_iterator& qiter) {
    Quat tmp = *qiter;
    string treg = "$v0";
    int type = (tmp.tabop1.type == strmap_type["int"]) ? 5 : 12;
    syntac_mips.push_back("li $v0, " + to_string(type) + " #" + to_string(qiter - syntac_quat.cbegin()));
    syntac_mips.push_back("syscall");
    genMips_findvar(tmp.op1, treg, false, qiter);// save value into stack or sreg
    qiter++;
    return;
}

void genMips_printf(vector<Quat>::const_iterator& qiter) {
    Quat tmp = *qiter;
    syntac_mips.push_back("la $a0, " + syntac_strlabeltab.find(tmp.op2)->second + " #" + to_string(qiter - syntac_quat.cbegin()));
    syntac_mips.push_back("li $v0, 4 #" + to_string(qiter - syntac_quat.cbegin()));
    syntac_mips.push_back("syscall #" + to_string(qiter - syntac_quat.cbegin()));
    if (tmp.op3 != "") {
        string treg = "$a0", type = to_string((tmp.op1 == "int") ? 1 : 11);
        genMips_findvar(tmp.op3, treg, true, qiter);//directly load value into a0
        syntac_mips.push_back("li $v0, " + type + " #" + to_string(qiter - syntac_quat.cbegin()));
        syntac_mips.push_back("syscall #" + to_string(qiter - syntac_quat.cbegin()));
    }
    syntac_mips.push_back("la $a0, str_n #" + to_string(qiter - syntac_quat.cbegin()));
    syntac_mips.push_back("li $v0, 4 #" + to_string(qiter - syntac_quat.cbegin()));
    syntac_mips.push_back("syscall #" + to_string(qiter - syntac_quat.cbegin()));
    qiter++;
    return;
}

void genMips_switch(vector<Quat>::const_iterator& qiter) {
    syntac_mips.push_back("#Switch #" + to_string(qiter - syntac_quat.cbegin()));
    qiter++;
    return;
}

void genMips_jump(vector<Quat>::const_iterator& qiter) {
    Quat tmp = *qiter;
    syntac_mips.push_back("j " + tmp.op1 + " #" + to_string(qiter - syntac_quat.cbegin()));
    qiter++;
    return;
}

void genMips_ret(vector<Quat>::const_iterator& qiter) {
    Quat tmp = *qiter;
    if (tmp.op2 == "main") {
        syntac_mips.push_back("li $v0, 10 #" + to_string(qiter - syntac_quat.cbegin()));
        syntac_mips.push_back("syscall #" + to_string(qiter - syntac_quat.cbegin()));
        qiter++;
        return;
    }
    else if (tmp.op1 != "") { // has some return value, load v1 into required var tmp.op1
        string retval = "$v1";
        genMips_findvar(tmp.op1, retval, true, qiter);
    }
    Tabinfo tabinfo = syntac_symtab[tmp.op2].front().second;
    // paras ret vars sregs tregs (current_sp)
    // write back global dirty vars
    Reginfo tmpreg;
    for (auto iter = gen_regs.cbegin(); iter != gen_regs.cend(); iter++) {
        tmpreg = iter->second;
        if (tmpreg.regname.data()[1] == 's' && tmpreg.level_name == "#global" && tmpreg.isdirty) {
            gen_regs[tmpreg.regname].needwb = true;//findreg will return a false so you can wb
            genMips_findvar(tmpreg.var_name + " " + tmpreg.level_name, tmpreg.regname, false, qiter, tmpreg.subscript);
        }
    }
    // load sreg value from stack into sreg
    if (gen_usedsreg.size() != 0) {
        for (auto iter = gen_usedsreg.cbegin(); iter != gen_usedsreg.cend(); iter++) {
            string targetreg = "$s" + to_string(*iter);
            int offset = 4 * (*iter) + 4 * 10;//treg is at the bottom
            syntac_mips.push_back("lw " + targetreg + ", " + to_string(offset) + "($fp) #" + to_string(qiter - syntac_quat.cbegin()));
        }
    }
    syntac_mips.push_back("lw $ra, " + to_string(4 * (10 + 8 + tabinfo.funcinfo.varcnt)) + "($fp) #" + to_string(qiter - syntac_quat.cbegin()));
    syntac_mips.push_back("addi $sp, $sp, " + to_string(4 * (10 + 8 + 1 + (int)tabinfo.funcinfo.paralist.size() + tabinfo.funcinfo.varcnt)) + " #" + to_string(qiter - syntac_quat.cbegin()));
    syntac_mips.push_back("move $fp, $sp");
    syntac_mips.push_back("jr $ra #" + to_string(qiter - syntac_quat.cbegin()));
    qiter++;
    return;
}

#endif // GenMips_opt

