#include "genMips.h" 
//#include "genMips_opt.h"
/*
C++ 11 STL
using _ for var/func name and upper case first letter for typedef
*/

string  filename;

int main() {
    string line;
    ifstream infile;
    cout << "Please enter the route of souce file(Press <Enter> for default test code):" << endl;
    getline(cin, filename);
    if (filename == "") {
        filename = "15061200_test.txt";
        cout << "Using default 15061200_test.txt" << endl;
    }
    infile.open(filename);
    if (!infile.is_open()) {
        cout << "File " << filename << " not found." << endl;
        cin.clear();
        cin.sync();
        cin.get();
        exit(EXIT_FAILURE);
    }
    while (getline(infile, line))
        lex_file.push_back(line);
    infile.close();
    cout << "Source file loaded." << endl;
    syntac_lineptr = lex_file.cbegin();
    lex_output = getsym(syntac_lineptr);
    /* 
    ＜程序＞ ::=［＜常量说明＞］［＜变量说明＞］{＜有返回值函数定义＞|＜无返回值函数定义＞}＜主函数＞
    */
    Funcinfo globalfunc;
    globalfunc.func_name = "#global";
    globalfunc.isreturn = false;
    globalfunc.re_type = "void";
    globalfunc.paralist = {};
    globalfunc.varcnt = 0;
    syntac_level_name = "#global";
    syntac_offset = 0;
    Tabinfo globaltab;
    globaltab.var_name = globalfunc.func_name;
    try
    {
        enter(syntac_level_name, globaltab, strmap_kind["func"], strmap_type["void"], globalfunc, {}, syntac_offset++);
        cons_declaration(syntac_lineptr, globalfunc);
        var_declaration(syntac_lineptr, globalfunc);
        function_declaration(syntac_lineptr);
        func_main(syntac_lineptr);
    }
    catch (const std::exception&)
    {
        cout<<"Something wrong with your input code"<<endl;
        printError(lex_errors, syntac_errors);
        cin.clear();
        cin.sync();
        cout << "Press any key to continue…";
        cin.get();
        return 0;
    }
    printSymtab();
    printQuat(Subfix);
    scanMultiuse();
    if (lex_errors.size() == 0 && syntac_errors.size() == 0) {
        //optimization()
        genMips();
        printMips(Subfix);
    }
    printError(lex_errors, syntac_errors);
    cin.clear();
    cin.sync();
    cout << "Press any key to continue…";
    cin.get();
    return 0;
}


bool lookuptab(Tabinfo& findresult, string level_name, string varname, int& position, set<int> required_type = {}, set<int> required_kind = {}) {
    // if find return true not find false loop up the var by given name in levelname and global
    Tabinfo tmpresult = {};
    // if func
    if (required_kind.find(strmap_kind["func"]) != required_kind.end()) {//func is one of the conditions
        if (syntac_symtab.find(varname) != syntac_symtab.end()) {
            findresult = syntac_symtab.find(varname)->second.front().second;
            // symtab = unorderedmap<string, vector<pair<string, tab>>>
            if (required_type.size() == 0 
                || required_type.find(strmap_type[findresult.funcinfo.re_type]) != required_type.end()) {
                //find the func with required return type
                position = 0;
                return true;
            }
        }
        if (required_kind.find(strmap_kind["func"]) == required_kind.begin()) {
            return false;//only func is required but not found above
        }
    }
    // if local
    if (level_name != "#global") {
        for (auto iter = syntac_symtab[level_name].cbegin();
            iter != syntac_symtab[level_name].cend();iter++) {
            if (iter->first == varname) {//same name in same level
                tmpresult = iter->second;
                if ( (required_type.size() != 0 
                         && required_type.find(tmpresult.type)==required_type.end()) ||
                     (required_kind.size() != 0 
                         && required_kind.find(tmpresult.kind)==required_kind.end() )) {
                    //but not same type and kind
                    if (tmpresult.kind == 2) {
                        //required not func but find a func
                        continue;
                    }
                    cout << "Found same name symbol in local but not requested kind/type" << endl;
                    findresult = tmpresult;
                    position = findresult.offset;
                    return true;
                }
                findresult = tmpresult;
                position = findresult.offset;
                return true;
            }
        }
    }
    // if global
    for (auto iter = syntac_symtab["#global"].cbegin();
        iter != syntac_symtab["#global"].cend(); iter++) {
        if (iter->first == varname) {
            tmpresult = iter->second;
            if ((required_type.size() != 0
                && required_type.find(tmpresult.type) == required_type.end()) ||
                (required_kind.size() != 0
                    && required_kind.find(tmpresult.kind) == required_kind.end())) {
                cout << "Found same name symbol in global but not requested kind/type" << endl;
                findresult = tmpresult;
                position = findresult.offset;
                return true;
            }
            findresult = tmpresult;
            position = findresult.offset;
            return true;
        }
    }
    return false;
}

int enter(string& level_name, Tabinfo input, int kind, int type, Funcinfo finfo = {}, Arrayinfo ainfo = {}, int off=-1) {
    // Find the var name in the given level_name and global level
    int position;
    Tabinfo findresult;
    static int varcount = 0;
    static string current_level = "#global";
    switch (kind) {
    case 2://func not allowed same name with other func
        if (lookuptab(findresult, syntac_level_name, input.var_name, position, {}, { 2 })) {
            syntac_errors.push_back({ "Duplicated Function Declaration. New Func declaration will be discared", lex_rownum });
            return -1;//error
        }
        break;
    case 5://parameter
    case 3://const
    case 1://array
    case 4://var
        if (lookuptab(findresult, syntac_level_name, input.var_name, position, {}, { 1,3,4,5 })) {
            if (findresult.level_name == syntac_level_name && findresult.kind != 2) {
                //any var/para/arr/const can have the same name with a func
                //not a func and same level then duplicated
                syntac_errors.push_back({"Duplicated var/para/array/const Declaration in the same level", lex_rownum });
                return -1;//error
            }
        }
        break;
    default: cout << "ERROR wrong input kind" << endl;
    }
    // if input is a func then level_neme = input.varname
    input.level_name = syntac_level_name;
    input.kind = kind;
    input.type = type;
    input.offset = off;
    input.funcinfo = finfo;
    input.arrinfo = ainfo;
    if (input.level_name != syntac_level_name) 
        cout << "ERROR wrong input level name" << endl;
    syntac_symtab[syntac_level_name].push_back(make_pair(input.var_name, input));
    if (input.level_name != input.var_name) { // no func
        if (input.kind == 1) { //array
            syntac_symtab[input.level_name].front().second.funcinfo.varcnt += input.arrinfo.max_capacity;
            syntac_offset += input.arrinfo.max_capacity - 1;//syntac_offset already ++ so minus 1
        }
        else if (input.kind == 4){ //var
            syntac_symtab[input.level_name].front().second.funcinfo.varcnt += 1;
        }
        //else // a parameter or a const do nothing
    }
    return 0;
}

void genQuat(string op, string op1 = "", string op2 = "", string op3 = "", Tabinfo tab1 = {}, Tabinfo tab2 = {}, Tabinfo tab3 = {}) {
    syntac_quat.push_back({ syntac_level_name, op, op1, op2, op3, tab1, tab2, tab3 });
}


void scanMultiuse() {
    vector<int> tmp;
    for (auto iter = syntac_quat.cbegin(); iter != syntac_quat.cend(); iter++) {
        string op1 = (*iter).op1, op2 = (*iter).op2, op3 = (*iter).op3;
        if (op1.size() > 0 && op1.front() == '#') {
            if (gen_multiuse.find(op1) == gen_multiuse.end())
                gen_multiuse[op1] = tmp;
            gen_multiuse[op1].push_back((int)(iter-syntac_quat.cbegin()));
            if (!Verbose&&gen_multiuse[op1].size() > 2) cout << op1+" more than 2 time use" << endl;
        }
        if (op2.size() > 0 && op2.front() == '#') {
            if (gen_multiuse.find(op2) == gen_multiuse.end())
                gen_multiuse[op2] = tmp;
            gen_multiuse[op2].push_back((int)(iter - syntac_quat.cbegin()));
            if (!Verbose&&gen_multiuse[op2].size() > 2) cout << op2 + " more than 2 time use" << endl;

        }
        if (op3.size() > 0 && op3.front() == '#') {
            if (gen_multiuse.find(op3) == gen_multiuse.end())
                gen_multiuse[op3] = tmp;
            gen_multiuse[op3].push_back((int)(iter - syntac_quat.cbegin()));
            if (!Verbose&&gen_multiuse[op3].size() > 2) cout << op2 + " more than 2 time use" << endl;
        }
    }

}

string gentmpvar() {
    syntac_tmpvar.push_back(make_pair("#" + to_string(syntac_tmpvar_cnt), syntac_level_name));
    return "#" + to_string(syntac_tmpvar_cnt++);
}

string genlabel() {
    return "Label_"+to_string(syntac_label_cnt++);
}

void save() {
    syntac_tmp_col = lex_colnum;
    syntac_tmp_lineptr = syntac_lineptr;
    lex_tmp_output = lex_output;
}

void load() {
    lex_colnum = syntac_tmp_col;
    syntac_lineptr = syntac_tmp_lineptr;
    lex_output = lex_tmp_output;
}

Enum2str getsym(vector<string>::const_iterator& lineptr) {
    Enum2str tmpoutput;
    static vector<int> codelinenum;
    lex_rownum = (int)(lineptr - lex_file.cbegin()) + 1;
    if (codelinenum.size() == 0 && !Verbose) {
        codelinenum.push_back(lex_rownum);
        printf("%-5d ", lex_rownum);
        cout << *syntac_lineptr << endl;
    }//split up for preventing two conditions from being exacuted in random order
    else if (!Verbose && codelinenum[codelinenum.size() - 1] < lex_rownum) {
        codelinenum.push_back(lex_rownum);
        printf("%-5d ", lex_rownum);
        cout << *syntac_lineptr << endl;
    }
    while(lineptr != lex_file.cend()) {
        tmpoutput = lexicalanalyze(lineptr);
        if (tmpoutput.sym == NOTYP) {
            cout << "Cannot identify! Tried automatic correction..." << endl;
            cout << "Following analysis correctness not guaranteed" << endl;
            //TODO: output auto corrected info ?
            return tmpoutput;
        }
        else if (tmpoutput.sym == ELINE) 
            continue;
        else if (tmpoutput.sym == ENDF) {
            break;
        }
        lex_tokencount++;
        lex_output = tmpoutput;
        return tmpoutput;
    }
    tmpoutput.sym = ENDF;
    if (end_flag) {
        return tmpoutput;
    }
    cout << "EOF! File not complete! Maybe you miss a } or sth" << endl;
    printError(lex_errors, syntac_errors);
    cin.clear();
    cin.sync();
    cout << "Press any key to continue…";
    cin.get();
    exit(0);
}

void printSyntacResult(string syntac, int count, bool iserror, string syntac_prefix) {
    if (Verbose) return;
    printf("This is a %s %s \n", syntac_prefix.data(), syntac.data());
}

bool isrequiredch(Enum2str input, string legal_sy) {
    if (input.key == legal_sy) {
        return true;
    }
    else {
        syntac_errors.push_back({ "Illegal Ch " + input.key + " Expected " + legal_sy , lex_rownum });
        return false;
    }
}

int cons_declaration(LinePtr& lineptr, Funcinfo& funcinfo) {
    /*
       ＜常量说明＞ ::= const＜常量定义＞;{ const＜常量定义＞;}
       ＜常量定义＞ ::=  int＜标识符＞＝＜整数＞{,＜标识符＞＝＜整数＞} | 
                        char＜标识符＞＝＜字符＞{,＜标识符＞＝＜字符＞}
    */
    save();
    int count = 0, tmpnum = 1;
    string syntacinfo, op2;
    Tabinfo tmptab;
    while (lex_output.sym == CONSTSY) {
        count=0;
        syntacinfo = "const ";
        lex_output = getsym(lineptr);
        if (lex_output.sym == INTSY || lex_output.sym == CHARSY) {
            tmptab.kind = strmap_kind["const"];
            tmptab.type = strmap_type[lex_output.key];
            tmptab.stat_line = lex_rownum;
            syntacinfo += lex_output.key;
            lex_output = getsym(lineptr);
            while (lex_output.sym == IDSY) {
                tmptab.var_name = lex_output.key;
                isrequiredch(getsym(lineptr), "=");
                lex_output = getsym(lineptr);
                if ((lex_output.sym == PLUS || lex_output.sym == MINUS) && tmptab.type == strmap_type["int"]) {
                    if (lex_output.sym == MINUS)
                        tmpnum = -1;
                    lex_output = getsym(lineptr);
                    if (lex_output.sym != NUM) {//error
                        syntac_errors.push_back({"Expected a number following +/- in the const declaration", lex_rownum});
                        break;
                    }
                    tmptab.num = lex_output.num * tmpnum;
                    op2 = to_string(tmptab.num);
                }
                else if (lex_output.sym == CH && tmptab.type == strmap_type["char"]) {
                    tmptab.ch = lex_output.key.data()[0];
                    string s(1, tmptab.ch);
                    op2 = s;
                }
                else if (lex_output.sym == NUM && tmptab.type == strmap_type["int"]) {
                    tmptab.num = lex_output.num;
                    op2 = to_string(tmptab.num);
                }
                else {
                    //TODO: skip to a set of symbol
                    break;
                }
                enter(syntac_level_name, tmptab, tmptab.kind, tmptab.type, {}, {});//no need to calculate offset for const
                genQuat("const "+map_type[tmptab.type], tmptab.var_name, op2);// op1 = op2
                count++;
                lex_output = getsym(lineptr);
                if (lex_output.key == ",") {
                    lex_output = getsym(lineptr);
                    continue;
                }
                else {
                    break; //already a new lexoutput
                }
            }
            if (reserved_name.find(lex_output.sym) != reserved_name.cend()) {//error
                syntac_errors.push_back({ "Cannot use reserved name " + lex_output.key + " as const name" , lex_rownum });
            }
        }
        else {
            syntac_errors.push_back({"Unknown type const declaration: " + lex_output.sym, lex_rownum});
        }
        isrequiredch(lex_output, ";");
        printSyntacResult(syntacinfo, count, false, "const_declaration");
        lex_output = getsym(lineptr);
    }
    return 0;
}

int var_declaration(LinePtr& lineptr, Funcinfo& funcinfo) {
    /*＜类型标识符＞::=  int | char
      ＜变量说明＞  ::= ＜变量定义＞;{＜变量定义＞;}
      ＜变量定义＞  ::= ＜类型标识符＞(＜标识符＞|＜标识符＞'['＜无符号整数＞']')
                            {,(＜标识符＞|＜标识符＞'['＜无符号整数＞']' )} 
      //＜无符号整数＞表示数组元素的个数，其值需大于0
    */
    //此处传入的funcinfo是为了生成四元式或许需要当前level函数信息
    int count = 0;
    string syntacinfo;
    Arrayinfo arrinfo;
    Tabinfo tmptab;
    while (lex_output.sym == INTSY || lex_output.sym == CHARSY) {
        save();
        count = 0;
        tmptab.type = strmap_type[lex_output.key];
        syntacinfo = lex_output.key+" ";
        lex_output = getsym(lineptr);
        while (lex_output.sym == IDSY) {
            arrinfo = {};
            tmptab.var_name = lex_output.key;
            tmptab.stat_line = lex_rownum;
            lex_output = getsym(lineptr);
            if (lex_output.key == "[") {
                lex_output = getsym(lineptr);
                if (lex_output.sym != NUM) {
                    syntac_errors.push_back({"Subscript:"+ lex_output.key +"\n Array declaration can only use unsigned int as subscript", lex_rownum});
                    break;
                }
                if (lex_output.num <= 0){
                    syntac_errors.push_back({"Subscript:" + lex_output.key+ "\n Array subscript should be bigger than 0",lex_rownum});
                    break;
                }
                arrinfo.max_capacity = lex_output.num;
                arrinfo.type = tmptab.type;
                isrequiredch(getsym(lineptr), "]");
                tmptab.kind = strmap_kind["array"];
                syntacinfo += "array ";
                lex_output = getsym(lineptr);
            }
            else if (lex_output.key == "(") {
                // this is a func declaration/func call
                // backtrack
                load();
                return 1;
            }
            else {
                tmptab.kind = strmap_kind["var"]; // a simple var
            }
            enter(syntac_level_name, tmptab, tmptab.kind, tmptab.type, {}, arrinfo, syntac_offset++);
            genQuat(map_type[tmptab.type]+" "+map_kind[tmptab.kind], tmptab.var_name, to_string(arrinfo.max_capacity));
            count++;
            syntacinfo += tmptab.var_name; // name
            if (lex_output.key == ",") {
                lex_output = getsym(lineptr);
                continue;
            }
            else {
                break; //already a new lexoutput
            }
        }
        if (reserved_name.find(lex_output.sym) != reserved_name.cend()) {//error
            syntac_errors.push_back({ "Cannot use reserved name " + lex_output.key + " as var/array name" ,lex_colnum});
        }
        isrequiredch(lex_output, ";");
        printSyntacResult(syntacinfo, count, false, "var_declaration");
        lex_output = getsym(lineptr);
    }
    return 0;
}

int function_declaration(LinePtr& lineptr) {
    /*
    ＜有返回值函数定义＞  ::= (int＜标识符＞ | char＜标识符＞)'('＜参数表＞')' '{'＜复合语句＞'}'
    ＜无返回值函数定义＞  ::= void＜标识符＞'('＜参数表＞')''{'＜复合语句＞'}'
    */
    int count = 0;
    string syntacinfo;
    Funcinfo funcinfo;
    Tabinfo tmptab;
    while (lex_output.sym == VOIDSY || lex_output.sym == INTSY || lex_output.sym == CHARSY) {
        save();
        syntacinfo = lex_output.key + " func ";
        funcinfo.paralist.clear();
        funcinfo.isreturn = (int)(lex_output.sym == INTSY || lex_output.sym == CHARSY);
        funcinfo.re_type = lex_output.key;
        funcinfo.varcnt = 0;
        lex_output = getsym(lineptr);
        if (lex_output.sym != IDSY) { //including ifsy whilesy main etc.
            if (lex_output.sym == MAIN) {
                // backtrack main func start
                load();
                return 1;
            }
            else if (reserved_name.find(lex_output.sym) != reserved_name.cend()) {//error
                syntac_errors.push_back({ "Cannot use reserved name " + lex_output.key + " as function name" ,lex_rownum});
            }
        }
        syntac_level_name = lex_output.key;
        syntac_offset = 0;
        tmptab.var_name = lex_output.key;
        tmptab.level_name = lex_output.key;
        tmptab.stat_line = lex_rownum;
        funcinfo.func_name = tmptab.var_name;
        genQuat(funcinfo.re_type+" func "+funcinfo.func_name);
        enter(syntac_level_name, tmptab, strmap_kind["func"], strmap_type[funcinfo.re_type], funcinfo, {}, syntac_offset++);
        isrequiredch(getsym(lineptr), "(");
        lex_output = getsym(lineptr);
        paralist(lineptr, funcinfo);
        isrequiredch(lex_output, ")");
        syntac_symtab[syntac_level_name].front().second.funcinfo = funcinfo;
        genQuat("func start", tmptab.var_name, "", ""); //start
        isrequiredch(getsym(lineptr), "{");
        lex_output = getsym(lineptr);
        func_compound(lineptr, funcinfo);
        isrequiredch(lex_output, "}");
        if (funcinfo.isreturn == false && syntac_quat.back().op != "Return") {
            genQuat("Return", "", syntac_level_name);//the last sentence is not a return then add one
        }
        else if (funcinfo.isreturn) {
            Quat tmpq = {syntac_level_name, "func start", tmptab.var_name, "", "",{},{},{} };
            auto iter = syntac_quat.cbegin();
            for (iter; iter != syntac_quat.cend(); iter++) {
                if (iter->op2 == syntac_level_name && iter->op == "Return") break;
            }
            if (iter == syntac_quat.cend()) {
                syntac_errors.push_back({ funcinfo.func_name + " need a return value", lex_rownum});
            }
        }
        genQuat("func end", tmptab.var_name, "", "", syntac_symtab[syntac_level_name][0].second);  //end
        lex_output = getsym(lineptr);
        printSyntacResult(syntacinfo, count, false, "function_declaration");
    }

    return 0;
}

int paralist(LinePtr& lineptr, Funcinfo& funcinfo) {
    /*
    ＜参数表＞  ::= ＜参数＞{,＜参数＞}| ＜空＞
    ＜参数＞    ::=  ＜类型标识符＞＜标识符＞
    ＜类型标识符＞  ::=  int | char
    */
    save();
    Tabinfo tmptab;
    string syntacinfo;
    tmptab.kind = strmap_kind["parameter"];
    while (lex_output.sym == INTSY || lex_output.sym == CHARSY) {
        int count = 0;
        tmptab.type = strmap_type[lex_output.key];
        syntacinfo = lex_output.key;
        lex_output = getsym(lineptr);
        if (lex_output.sym != IDSY) {
            syntac_errors.push_back({"Illegal parameter name: "+lex_output.key,lex_rownum});
            continue;
        }
        tmptab.var_name = lex_output.key;
        tmptab.level_name = funcinfo.func_name;
        tmptab.stat_line = lex_rownum;
        enter(syntac_level_name, tmptab, strmap_kind["parameter"], tmptab.type, {}, {}, syntac_offset++);
        funcinfo.paralist.push_back(make_pair(tmptab.type, tmptab.var_name));
        genQuat(map_type[tmptab.type]+" "+map_kind[tmptab.kind], tmptab.var_name);//parameter
        lex_output = getsym(lineptr);
        printSyntacResult(syntacinfo, 0, false, "parameter of "+funcinfo.func_name);
        if (lex_output.sym == COMMA) {
            lex_output = getsym(lineptr);
            continue;
        }
        else if (lex_output.sym == RPAR) {
            return 0;
        }
        else {
            syntac_errors.push_back({"Illegal paralist form, expected ',' or ')'",lex_rownum});
            return -1;
        }
    }
    return -1;
}

int func_compound(LinePtr& lineptr, Funcinfo& funcinfo) {
    /*
    ＜复合语句＞   ::=  ［＜常量说明＞］［＜变量说明＞］＜语句列＞
    */
    save();
    string syntac_info = "";
    if (lex_output.sym == CONSTSY) {
        cons_declaration(lineptr, funcinfo);
    }
    if (lex_output.sym == INTSY || lex_output.sym == CHARSY) {
        var_declaration(lineptr, funcinfo);
    }
    while (lex_output.key != "}") {
        if (statement(lineptr) < 0) {
            syntac_errors.push_back({ "Wrong statement in func compound" ,lex_rownum});
            lex_output = getsym(lineptr);//skip reading
        }
    }
    printSyntacResult(syntac_info, 0, false, "compound sentences");
    return 0;
}

int func_main(LinePtr& lineptr) {
    /*
    ＜主函数＞ ::= void main'('')''{'＜复合语句＞'}'
    */
    save();
    string syntac_info = "";
    Tabinfo tmptab;
    Funcinfo mainfunc_info;
    mainfunc_info.func_name = "main";
    mainfunc_info.isreturn = false;
    mainfunc_info.re_type = "void";
    mainfunc_info.paralist = {};
    mainfunc_info.varcnt = 0;
    syntac_level_name = "main";
    syntac_offset = 0;
    tmptab.var_name = "main";
    tmptab.stat_line = lex_rownum;
    enter(syntac_level_name, tmptab, strmap_kind["func"], strmap_type["void"], mainfunc_info, {}, syntac_offset++);
    genQuat(mainfunc_info.re_type + " func " + mainfunc_info.func_name);
    isrequiredch(lex_output, "void");
    isrequiredch(getsym(lineptr), "main");
    isrequiredch(getsym(lineptr), "(");
    isrequiredch(getsym(lineptr), ")");
    genQuat("func start", tmptab.var_name, "", ""); //start
    isrequiredch(getsym(lineptr), "{");
    lex_output = getsym(lineptr);
    func_compound(lineptr, mainfunc_info);
    isrequiredch(lex_output, "}");
    if (mainfunc_info.isreturn == false && syntac_quat.back().op != "Return") {
        genQuat("Return", "", syntac_level_name);//the last sentence is not a return then add one
    }
    genQuat("func end", tmptab.var_name, "", "", syntac_symtab[syntac_level_name][0].second); //end
    end_flag = true;
    lex_output = getsym(lineptr);
    printSyntacResult(syntac_info, 0, false, "main func");
    if (lex_output.sym != ENDF) {
        syntac_errors.push_back({"The code file is supposed to end",lex_rownum});
        cout << "The code file is supposed to end" << endl;
        cout << "The rest of the file won't be compiled" << endl;
        return -1;
    }
    return 0;
}

int func_call(LinePtr& lineptr, bool needret, string& result, string& resulttype) {
    /*
    ＜有返回值函数调用语句＞ ::= ＜标识符＞'('＜值参数表＞')'
    ＜无返回值函数调用语句＞ ::= ＜标识符＞'('＜值参数表＞')'
    ＜值参数表＞   ::= ＜表达式＞{,＜表达式＞}｜＜空＞
    */
    save();
    int position = 0, currentfunccount = funccount;
    Tabinfo findresult;
    string syntac_info = "";
    if (lex_output.sym == IDSY
        && lookuptab(findresult, syntac_level_name, lex_output.key, position, {}, {strmap_kind["func"]} )) {
        if (findresult.funcinfo.isreturn == false && needret) {
            syntac_errors.push_back({ "Called a void function while a return value is needed" , lex_rownum});
            return -1;
        }
        syntac_info = lex_output.key;
        isrequiredch(getsym(lineptr), "(");
        lex_output = getsym(lineptr);
        //syntac_funcnum.push_back(make_pair(findresult.funcinfo.func_name, funccount));
        //every func call has a unique NO
        funccount++;//add before para list call another func
        value_paralist(lineptr, findresult.funcinfo, currentfunccount);
        isrequiredch(lex_output, ")");
        //syntac_funcnum.pop_back();//incase value paralist call another func
    }
    else {
        syntac_errors.push_back({ "Function not found" , lex_rownum});
        return -1;
    }
    resulttype = findresult.funcinfo.re_type;
    result = gentmpvar();
    genQuat("call "+findresult.funcinfo.re_type+" func "+to_string(currentfunccount), findresult.var_name, result, syntac_level_name);
    lex_output = getsym(lineptr);
    printSyntacResult(syntac_info, 0, false, "func call");
    return 0;
}

int value_paralist(LinePtr& lineptr, Funcinfo& funcinfo, int funcNO) {
    /*
     ＜值参数表＞   ::= ＜表达式＞{,＜表达式＞}｜＜空＞
    */
    save();
    string syntac_info = "";
    int count = 0;
    string tmpresult, resulttype="";
    while (lex_output.key != ")") {
        expression(lineptr, tmpresult, resulttype);
        count++;
        if (count > funcinfo.paralist.size() ) {//skip them until find a ')'
            syntac_errors.push_back({ "Too many parameters ! Expected paralist size:" + to_string(funcinfo.paralist.size()) + "Current count: " + to_string(count), lex_rownum});
            continue;
        }
        else if (funcinfo.paralist[count - 1].first != strmap_type[resulttype]) {//wrong para type but skip
            //here count-1 cause in paralist subscript start from 0
            syntac_errors.push_back({ "Wrong parameter type, expected: " + map_type[funcinfo.paralist[count - 1].first] + "Given input: " + resulttype ,lex_rownum});
        }
        genQuat("push "+resulttype, funcinfo.paralist[count - 1].second, tmpresult, syntac_level_name+" "+to_string(funcNO)+" "+funcinfo.func_name); // push result into corresponding para
        if (lex_output.key == ",") {
            lex_output = getsym(lineptr);
        }
        else if (lex_output.key != ")") {
            syntac_errors.push_back({"Expected ')' at the end of function call",lex_rownum});
            return -1;
        }
    }
    if (count < funcinfo.paralist.size()) {//error
        syntac_errors.push_back({ "Too few parameters ! Expected paralist size:" + to_string(funcinfo.paralist.size()) + "Current count: " + to_string(count) ,lex_rownum});
    }
    printSyntacResult(syntac_info, count, false, "value paralist");
    return 0;
}

int expression(LinePtr& lineptr, string& result, string& resulttype) {
    /*
    ＜表达式＞ ::= ［＋｜－］＜项term＞{＜加法运算符＞＜项＞}   //[+|-]只作用于第一个<项>
    ＜项＞     ::= ＜因子factor＞{＜乘法运算符＞＜因子＞}
    ＜因子＞   ::= ＜标识符＞｜＜标识符＞'['＜表达式＞']'|'('＜表达式＞')'｜＜整数＞|＜字符＞｜＜有返回值函数调用语句＞
    */
    save();
    string syntac_info = "", op1 = "", op2 = "", op3 = "", sign=""; 
    int tmpnum = 1, count = 0;
    if (lex_output.sym == PLUS || lex_output.sym == MINUS) {
        if (lex_output.sym == MINUS) {
            tmpnum = -1;
        }
        sign = lex_output.key;
        lex_output = getsym(lineptr);
    }
    int status = term(lineptr, tmpnum, op1, resulttype);//only when a char without any +/- resulttype would be char
    if (status >= 0 && sign == "-") {//sign == -
        resulttype = "int";
        if (isdigit(op1.front()) || op1.front() == '-' || op1.front() == '+'){
            op1 = to_string(-1 * stoi(op1));
        }
        else if (op1.front() == '\'') {
            op1 = to_string(-1 * int(op1.data()[1]));
        }
        else {
            op3 = op1;
            op1 = gentmpvar();
            genQuat(sign, op1, "$0", op3); // 0-op3
        }
    }
    else if (status >= 0 && sign == "+") {
        resulttype = "int";
    }
    count++;
    while (lex_output.sym == PLUS || lex_output.sym == MINUS) {
        sign = lex_output.key;
        lex_output = getsym(lineptr);
        op2 = op1;
        term(lineptr, 1, op3, resulttype);
        count++;
        op1 = gentmpvar();
        genQuat(sign, op1, op2, op3);
        resulttype = "int";
    }
    result = op1;
    printSyntacResult(syntac_info, count, false, "expression");
    return status;
}

int term(LinePtr& lineptr, int isminus, string& result, string& resulttype) {
    /*
    ＜项＞  ::= ＜因子factor＞{＜乘法运算符＞＜因子＞}
    */
    save();
    string syntac_info = "", op="", op1 = "", op2 = "", op3 = "";
    int count = 1;
    int status = 0;
    status = factor(lineptr, op1, resulttype);
    while (lex_output.key == "*" || lex_output.key == "/") {
        op = lex_output.key ;
        op2 = op1; // store current output
        lex_output = getsym(lineptr);
        status = factor(lineptr, op3, resulttype); // load a new factor
        op1 = gentmpvar();
        genQuat(op, op1, op2, op3); // op1 = op2 */ op3
        count++;
        resulttype = "int";
    };
    result = op1;
    printSyntacResult(syntac_info, count, false, "term");
    return status;
}

int factor(LinePtr& lineptr, string& result, string& resulttype) {
    /*
    ＜因子＞:: = ＜标识符＞｜＜标识符＞'['＜表达式＞']' | '('＜表达式＞')'
                         ｜＜整数＞ | ＜字符＞｜＜有返回值函数调用语句＞
    */
    save();
    string syntac_info = "", subscript, tmpname;
    Tabinfo findresult;
    int tmpnum = 1, position = 0, status = 0;
    if (lex_output.sym == IDSY) {
        tmpname = lex_output.key;
        lex_output = getsym(lineptr);
        syntac_info = "";
        if (lex_output.key == "[") {
        // array
            lex_output = getsym(lineptr);
            status = expression(lineptr, subscript, resulttype);
            isrequiredch(lex_output,"]");
            if (resulttype == "char") {
                syntac_errors.push_back({"Array subscript can only be a int",lex_rownum});
                status = -1;
            }
            if (lookuptab(findresult, syntac_level_name, tmpname, position, {}, 
                { strmap_kind["array"] } ) == false) {
                syntac_errors.push_back({"Var/Array name not found: " + tmpname,lex_rownum});
                status = -1;
            }
            else if (findresult.kind != strmap_kind["array"]) {
                syntac_errors.push_back({ "Array not found but a wrong kind symbol with same name",lex_rownum});
                status = -1;
            }
            if (isdigit(subscript.front()) || subscript.front() == '-' || subscript.front() == '+') {
                if (stoi(subscript) >= findresult.arrinfo.max_capacity || stoi(subscript) < 0) {//error
                    syntac_errors.push_back({ "Array subscript out of range. Legal subscript range: 0-" + to_string(findresult.arrinfo.max_capacity - 1) + " Input: " + subscript , lex_rownum });
                    status = -1;
                }
            }
            lex_output = getsym(lineptr);
            syntac_info = "array";
            result = gentmpvar();
            resulttype = map_type[findresult.arrinfo.type];
            genQuat("=[]", result, findresult.var_name, subscript, {}, findresult, {});
        }
        else if (lex_output.key == "(") {
        // func call and backtrack
            load();
            status = func_call(lineptr, true, result, resulttype);
            syntac_info = "function call";
            printSyntacResult(syntac_info, 0, false, "factor of");
            return 1;
        }
        else {
        // simple var
            if (lookuptab(findresult, syntac_level_name, tmpname, position, {}, 
                { strmap_kind["const"], strmap_kind["var"] , strmap_kind["parameter"] }) == false) {
                cout << "ERROR this is not a var/para/const" << endl;//this shouldn't happen
                status = -1;
            }
            else if (findresult.kind != strmap_kind["const"] &&
                     findresult.kind != strmap_kind["var"] && 
                     findresult.kind != strmap_kind["parameter"]) {
                syntac_errors.push_back({ "Required kind var/para/const wiht given name not found" , lex_rownum});
                status = -1;
            }
            if (findresult.kind == 3) {
                syntac_info = (findresult.type == strmap_type["int"]) ? "const int" : "const char";
                string s(1, findresult.ch);
                result = (findresult.type == strmap_type["int"]) ? to_string(findresult.num) : "'" + s + "'";
            }
            else {
                syntac_info = "simple var/para";
                if (findresult.level_name == "#global") {//Special for global
                    string tmpvar = gentmpvar();
                    genQuat("=", tmpvar, tmpname + " " + syntac_level_name, "");
                    result = tmpvar;
                }
                else {// save for later genmips find the var in tab
                    result = tmpname + " " + syntac_level_name;
                }
            }
            resulttype = map_type[findresult.type];
        }
    }
    else if (lex_output.key == "(") {
        lex_output = getsym(lineptr);
        status = expression(lineptr, result, resulttype);
        resulttype = "int";//regarded as an expression
        isrequiredch(lex_output, ")");
        lex_output = getsym(lineptr);
        syntac_info = "(expression)";
    }
    else if (lex_output.sym == NUM) {
        result = to_string(lex_output.num);
        syntac_info = "number";
        resulttype = "int";
        lex_output = getsym(lineptr);
    }
    else if (lex_output.sym == CH) {
        result = "'"+lex_output.key+"'";
        syntac_info = "char";
        resulttype = "char";
        lex_output = getsym(lineptr);
    }
    else if (lex_output.sym == PLUS || lex_output.sym == MINUS) {
        tmpnum = stoi(lex_output.key+"1");
        lex_output = getsym(lineptr);
        if (lex_output.sym == NUM) {
            result = to_string(lex_output.num * tmpnum);
            syntac_info = "number";
            resulttype = "int";
            lex_output = getsym(lineptr);
        }
        else {
            syntac_errors.push_back({"Input: "+lex_output.key+"\n+/- in a factor must followed by a num",lex_rownum});
            lex_output = getsym(lineptr);
            status = -1;
        }
    }
    else {
        syntac_errors.push_back({"Input: "+lex_output.key+"\nUnknown factor type",lex_rownum});
        lex_output = getsym(lineptr);
        status = -1;
    }
    printSyntacResult(syntac_info, 0, false, "factor");
    return status;
}

int statement(LinePtr& lineptr) {
    /*
    ＜语句＞    ::= ＜条件语句＞｜＜循环语句＞| ＜情况语句＞|'{'＜语句列＞'}'| 
                   ＜有返回值函数调用语句＞;|＜无返回值函数调用语句＞;｜＜赋值语句＞;
                   ｜＜读语句＞;｜＜写语句＞;｜＜空＞;｜＜返回语句＞;
    ＜语句列＞   ::= ｛＜语句＞｝//可以为0个语句
    */
    save();
    string syntac_info = "", result, resulttype;// just two para to use func_call()
    //actually not useful cause func call's return value no use
    int subscript = -1;
    if (lex_output.key == "{") {
        lex_output = getsym(lineptr);
        while (lex_output.key != "}") {
            if (statement(lineptr) < 0) {
                syntac_errors.push_back({ "Wrong Statement in multi statements" ,lex_rownum});//skip reading
                lex_output = getsym(lineptr);
            }
        }
        isrequiredch(lex_output,"}");
        lex_output = getsym(lineptr);
        syntac_info = "multi";
    }
    else if (lex_output.sym == IFSY) {
        sta_if(lineptr);
        syntac_info = "if";
    }
    else if (lex_output.sym == WHILESY) {
        sta_while(lineptr);
        syntac_info = "while";
    }
    else if (lex_output.sym == SWITCHSY) {
        sta_switch(lineptr);
        syntac_info = "switch";
    }
    else if (lex_output.sym == IDSY) {
        save(); //saved here then next sym is still this IDSY
        lex_output = getsym(lineptr);
        if (lex_output.key == "(") {
            load();
            func_call(lineptr, false, result, resulttype);
            syntac_info = "function call";
            printSyntacResult(syntac_info, 0, false, "statement ");
        }
        else {
            load();
            sta_assign(lineptr);
            syntac_info = "assign";
            printSyntacResult(syntac_info, 0, false, "statement");
        }
        
        isrequiredch(lex_output, ";");
        lex_output = getsym(lineptr);
        return 0;
    }
    else if (lex_output.sym == PRINTFSY) {
        sta_printf(lineptr);
        isrequiredch(lex_output, ";");
        lex_output = getsym(lineptr);
        syntac_info = "printf";
    }
    else if (lex_output.sym == SCANFSY) {
        sta_scanf(lineptr);
        isrequiredch(lex_output, ";");
        lex_output = getsym(lineptr);
        syntac_info = "scanf";
    }
    else if (lex_output.sym == RETURNSY) {
        sta_return(lineptr);
        isrequiredch(lex_output, ";");
        lex_output = getsym(lineptr);
        syntac_info = "return";
    }
    else if (lex_output.key == ";") {
        lex_output = getsym(lineptr);
        syntac_info = "empty";
    }
    else {
        syntac_errors.push_back({"Unknown Statement type",lex_rownum});
        return -1;
    }

    printSyntacResult(syntac_info, 0, false, "statement");
    return 0;
}

int sta_assign(LinePtr& lineptr) {
    /*
    ＜赋值语句＞ ::= ＜标识符＞＝＜表达式＞|＜标识符＞'['＜表达式＞']'=＜表达式＞
    */
    save();
    int position, status = 0;
    Tabinfo findresult;
    string syntac_info = "", tmpname;
    string result = "", resulttype = "", op1, op2, op3;
    if (lex_output.sym == IDSY) {
        tmpname = lex_output.key;
        lex_output = getsym(lineptr);
        if (lex_output.key == "[") {
            lex_output = getsym(lineptr);
            status = expression(lineptr, op2, resulttype);
            isrequiredch(lex_output, "]");
            if (resulttype == "char") {
                syntac_errors.push_back({ "Array subscript can only be a int",lex_rownum });
                status = -1;
            }
            op1 = tmpname;
            if (lookuptab(findresult, syntac_level_name, tmpname, position, {}, { strmap_kind["array"] }) == false) {
                syntac_errors.push_back({"Array not found",lex_rownum});
                status = -1;
            }
            else if (findresult.kind != strmap_kind["array"]) {
                syntac_errors.push_back({ "Array not found but a wrong kind symbol with same name" , lex_rownum });
            }
            isrequiredch(getsym(lineptr), "=");
            lex_output = getsym(lineptr);
            status = status + expression(lineptr, op3, resulttype);
            if (strmap_type[resulttype] != findresult.arrinfo.type) {
                syntac_errors.push_back({"Cannot assign a "+ resulttype +" value to a "+map_type[findresult.arrinfo.type],lex_rownum});
                status = -1;
            }
            if (isdigit(op2.front()) || op2.front() == '-' || op2.front() == '+') {
                if (stoi(op2) >= findresult.arrinfo.max_capacity || stoi(op2) < 0) {//error
                    syntac_errors.push_back({ "Array subscript out of range. Legal subscript range: 0-" + to_string(findresult.arrinfo.max_capacity-1) + " Input: " + op2 , lex_rownum });
                }
            }
            genQuat("[]=", op1, op2, op3, findresult); //op1[op2]=op3
        }
        else if (lookuptab(findresult, syntac_level_name, tmpname, position, {}, 
                           { strmap_kind["var"], strmap_kind["parameter"], strmap_kind["const"] })
                 && lex_output.key == "=" ){
            if (findresult.kind != strmap_kind["const"] &&
                findresult.kind != strmap_kind["var"] &&
                findresult.kind != strmap_kind["parameter"]) {
                syntac_errors.push_back({ "Required kind var/para/const wiht given name not found" ,lex_rownum});
                status = -1;
            }
            if (findresult.kind == strmap_kind["const"]) {//error
                syntac_errors.push_back({ "Cannot assign another value to a const ",lex_rownum});
            }
            op1 = tmpname;
            lex_output = getsym(lineptr);
            expression(lineptr, op2, resulttype);
            if (strmap_type[resulttype] != findresult.type) {
                syntac_errors.push_back({ "Cannot assign a " + resulttype + " value to a " + map_type[findresult.type], lex_rownum });
                status = -1;
            }
            genQuat("=", op1+" "+syntac_level_name, op2, "", findresult); //tab1 = findresult
        }
        else {
            syntac_errors.push_back({"Required symbol not found or assign statement incomplete",lex_rownum});
            status = -1;
        }
        return status;
    }
    else {
        syntac_errors.push_back({"Illegal left value: "+lex_output.key,lex_rownum});
        status = -1;
    }
    return status;
}

int condition(LinePtr& lineptr, string& label) {
    /*
    ＜关系运算符＞  ::=  <｜<=｜>｜>=｜!=｜==
    ＜条件＞  ::=  ＜表达式＞＜关系运算符＞＜表达式＞｜＜表达式＞ //表达式为0条件为假，否则为真
    */
    save();
    string syntac_info = "", op = "!="; //default is result_bool != 0 
    string result_bool = "", result_bool2 = "$0", resulttype = "", resulttype2 = "";
    expression(lineptr, result_bool, resulttype);
    if (lex_output.sym <= 121 && lex_output.sym >= 116) {
        op = lex_output.key;
        lex_output = getsym(lineptr);
        expression(lineptr, result_bool2, resulttype2);
    }
    if (resulttype=="char" || resulttype2 == "char") {//error only both int allowed
        syntac_errors.push_back({ "Wrong op type op1 is " + resulttype + " op2 is " + resulttype2 ,lex_rownum });
    }
    genQuat(op, label, result_bool, result_bool2);
    printSyntacResult("", 0, false, "condition");
    return 0;
}

int sta_switch(LinePtr& lineptr) {
    /*
    ＜情况语句＞  ::=  switch '('＜表达式＞')' '{'＜情况表＞＜缺省＞ '}'
    ＜情况表＞   ::=  ＜情况子语句＞{＜情况子语句＞}
    ＜情况子语句＞  ::=  case＜常量＞：＜语句＞
    ＜常量＞   ::=  ＜整数＞|＜字符＞
    ＜缺省＞   ::=  default : ＜语句＞|＜空＞
    */
    save();
    string syntac_info = "", lastlabel = genlabel();
    string result, resulttype = "";
    if (lex_output.sym == SWITCHSY) {
        lex_output = getsym(lineptr);
        isrequiredch(lex_output, "(");
        lex_output = getsym(lineptr);
        expression(lineptr, result, resulttype);
        isrequiredch(lex_output, ")");
        genQuat("switch", result, resulttype);
        isrequiredch(getsym(lineptr), "{");
        lex_output = getsym(lineptr);
        case_list(lineptr, result, resulttype, lastlabel);
        def_list(lineptr);
        isrequiredch(lex_output, "}");
        genQuat(lastlabel + ":");

        lex_output = getsym(lineptr);
        return 0;
    }
    else {
        syntac_errors.push_back({"Not a switch statement",lex_rownum});
        return -1;
    }
}

int case_list(LinePtr& lineptr, const string& inputcase, const string& inputtype, const string& lastlabel) {
    /*
    ＜情况表＞   ::=  ＜情况子语句＞{＜情况子语句＞}
    ＜情况子语句＞  ::=  case＜常量＞：＜语句＞
    ＜常量＞   ::=  ＜整数＞|＜字符＞
    */
    string tmpcase, nextlabel, s="NOCASE";
    int consnum = 0, tmpnum = 1;
    vector<string> cases;
    while (lex_output.sym == CASESY) {
        lex_output = getsym(lineptr);
        if (lex_output.sym == PLUS || lex_output.sym == MINUS) {
            tmpnum = stoi(lex_output.key + "1");
            lex_output = getsym(lineptr);
            if (lex_output.sym == NUM) {
                consnum = tmpnum * lex_output.num;
            }
            else if (lex_output.sym == CH) {
                syntac_errors.push_back({"+/- should be followed by a number: "+lex_output.key, lex_rownum});
                return -1;
            }
            else {
                syntac_errors.push_back({"Case list expected a number here",lex_rownum});
                return -1;
            }
        }
        else if (lex_output.sym == NUM) {
            consnum = lex_output.num;
        }
        else if (lex_output.sym == CH) {
            consnum = lex_output.key.data()[0];
            s.clear();
            s.push_back('\'');
            s.push_back(consnum);
            s.push_back('\'');
        }
        else {
            syntac_errors.push_back({"Illegal case list value: "+lex_output.key,lex_rownum});
            return -1;
        }
        tmpcase = (lex_output.sym == CH)? s:to_string(consnum);
        if (lex_output.sym == CH && inputtype != "char" ||
            lex_output.sym == NUM && inputtype != "int") {//error
            syntac_errors.push_back({ "Switch input type (" + inputtype + ") should be the same type with const case", lex_rownum });
        }
        if (find(cases.cbegin(), cases.cend(), tmpcase) != cases.cend()) {
            syntac_errors.push_back({ "Duplicated cases: " + tmpcase, lex_rownum});
        }
        cases.push_back(tmpcase);
        isrequiredch(getsym(lineptr), ":");
        nextlabel = genlabel();
        genQuat("==", nextlabel, inputcase, tmpcase); //if op2==op3 false then B
        lex_output = getsym(lineptr);
        statement(lineptr);
        genQuat("GOTO", lastlabel);
        genQuat(nextlabel + ":");
        printSyntacResult("", 0, false, "case_sta");
    }
    printSyntacResult("", 0, false, "case_list");
    return 0;
}

int def_list(LinePtr& lineptr) {
    /*
    ＜缺省＞   ::=  (default : ＜语句＞)|＜空＞
    */
    if (lex_output.sym == DEFAULTSY) {
        isrequiredch(getsym(lineptr), ":");
        lex_output = getsym(lineptr);
        if (lex_output.key == "}") {//error
            syntac_errors.push_back({ "Empty default case not allowed", lex_rownum});
            return -1;
        }
        statement(lineptr);
        printSyntacResult("", 0, false, "default_case");
        return 0;
    }
    else {
        //no dafault
        printSyntacResult("", 0, false, "no default_case");
        return 0;
    }
}

int sta_if(LinePtr& lineptr) {
    /*
    ＜条件语句＞ ::= if '('＜条件＞')'＜语句＞
    */
    string result, label = genlabel();
    if (lex_output.sym == IFSY) {
        isrequiredch(getsym(lineptr),"(");
        lex_output = getsym(lineptr);
        condition(lineptr, label);
        isrequiredch(lex_output, ")");
        lex_output = getsym(lineptr);
        statement(lineptr);
        genQuat(label+":"); // if condition is not satisfied
        return 0;
    }
    else {
        syntac_errors.push_back({"Not a if statement",lex_rownum});
        return -1;
    }
}

int sta_while(LinePtr& lineptr) {
    /*
    ＜循环语句＞ ::=  while '('＜条件＞')'＜语句＞
    */
    string result, label1 = genlabel(), label2 = genlabel();
    if (lex_output.sym == WHILESY) {
        isrequiredch(getsym(lineptr), "(");
        lex_output = getsym(lineptr);
        genQuat(label1 + ":");
        condition(lineptr, label2);
        isrequiredch(lex_output, ")");
        lex_output = getsym(lineptr);
        statement(lineptr);
        genQuat("GOTO", label1);
        genQuat(label2 + ":");
        return 0;
    }
    else {
        return -1;
    }
}

int sta_return(LinePtr& lineptr) {
    /*
    ＜返回语句＞   ::=  return['('＜表达式＞')']  
    */
    string result = "", resulttype = "", expected_type="";
    expected_type = syntac_symtab.find(syntac_level_name)->second.front().second.funcinfo.re_type;
    if (lex_output.sym == RETURNSY) {
        lex_output = getsym(lineptr);
        if (lex_output.key == ";") {
            genQuat("Return", "", syntac_level_name);
            if (expected_type != "void") {
                syntac_errors.push_back({ "Wrong return value type! Expected: " + expected_type + " Given: void", lex_rownum });
            }
            return 0;
        }
        isrequiredch(lex_output,"(");
        lex_output = getsym(lineptr);
        expression(lineptr, result, resulttype);
        isrequiredch(lex_output, ")");
        if (expected_type != resulttype) {//error
            syntac_errors.push_back({ "Wrong return value type! Expected: "+expected_type +" Given: "+resulttype, lex_rownum });
        }
        genQuat("Return", result, syntac_level_name);
        lex_output = getsym(lineptr);
        return 0;
    }
    else {
        return -1;
    }
}

int sta_printf(LinePtr& lineptr) {
    /*
    ＜字符串＞   ::=  "｛十进制编码为32,33,35-126的ASCII字符｝"
    ＜写语句＞    ::= printf '(' ＜字符串＞,＜表达式＞ ')'
                   | printf '('＜字符串＞ ')'
                   | printf '('＜表达式＞')'
    */
    save();
    string str, resulttype = "", result = "";
    if (lex_output.sym == PRINTFSY) {
        isrequiredch(getsym(lineptr),"(");
        lex_output = getsym(lineptr);
        if (lex_output.sym == STR) {
            str = strreplace(lex_output.key, '\\', "\\\\");
            lex_output = getsym(lineptr);
            printSyntacResult("", 0, false, "string");
            if (lex_output.key == ",") {
                lex_output = getsym(lineptr);
                expression(lineptr, result, resulttype);
            }
            genQuat("Printf", resulttype, str, result); //if result is "" then just str
        }
        else {
            expression(lineptr, result, resulttype);
            genQuat("Printf", resulttype, "", result);
        }
        isrequiredch(lex_output, ")");
        lex_output = getsym(lineptr);
        return 0;
    }
    else {
        syntac_errors.push_back({"Not a printf statement", lex_rownum});
        return -1;
    }
}

int sta_scanf(LinePtr& lineptr) {
    /*
    ＜读语句＞    ::=  scanf '('＜标识符＞{,＜标识符＞}')'
    */
    int count = 0, position=0;
    Tabinfo findresult;
    if (lex_output.sym == SCANFSY) {
        isrequiredch(getsym(lineptr), "(");
        lex_output = getsym(lineptr);
        do {
            if (count != 0) {
                lex_output = getsym(lineptr);
            }
            if (lex_output.sym == IDSY) {
                if (lookuptab(findresult, syntac_level_name, lex_output.key, position, {},
                    { strmap_kind["var"], strmap_kind["parameter"] }) == false) {
                    syntac_errors.push_back({"Scanf var/para not found: " + lex_output.key, lex_rownum});
                    return -1;
                }
                else if (findresult.kind != strmap_kind["var"] &&
                    findresult.kind != strmap_kind["parameter"]) {
                    syntac_errors.push_back({ "Scanf required var/para not found, something else with same name found: "+map_type[findresult.type]+" "+findresult.var_name, lex_rownum});
                    return -1;
                }
                genQuat("Scanf", lex_output.key+" "+findresult.level_name, "", "", findresult);
                lex_output = getsym(lineptr);
                count++;
            }
            else {
                return -1;
            }
        } while (lex_output.key == ",");
        isrequiredch(lex_output, ")");
        lex_output = getsym(lineptr);
        return 0;
    }
    else {
        return -1;
    }
}