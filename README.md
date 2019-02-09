# BUAA-Compiler-Design
### 0x00 Introduction
Course work (a C0 compiler) for BUAA B3I063410 Compiler Design.  
Students will be required to design and implement a C0 compiler in 6 weeks, step by step. From lexical analysis, syntax analysis to semantic analysis (Quaternary mid-code generation included) and finally, to mips code generation and optimization.  
### 0x01 File Structure
- Source Code 
  - Final release version
  - No optimization version
- Documents  
  - My design thoughts
  - Software Documentation
- Test input  
  - Some test programs
- C0 grammer.txt
  - Detailed C0 grammer requirements
### 0x02 Usage & Settings
#### Usage
This is a **Visual Studio 2017** projects, you could open it directly with VS17. 
Once you start the program, you could press <enter> directly to compile the default `15061200_test.txt` or you could input the full path
  of your own test program.
The compiler's output should include:
   
File Name    | Contents  
------------ | ------------  
output_after(before).asm | Mips code    
syntac_output_after(before).txt | Quaternary Mid Code
  
After that, you could run the generated mips code with `Mars_refreshfix.jar`.
#### Settings
In `SyntacticAnalyze.cpp`, you can decide whether to optimize the output or not.
```C++
#include "genMips.h" 
//#include "genMips_opt.h"
```
In `myconst.h`, it's on your choice that
```C++
const bool Verbose = true;// if true then no detailed syntac info output in console 
const string Subfix = "before";// set this to after and your output file will be named with subfix '_after'
const string Newline = "";//Newline = "\\n" if you want to beautify the mips runtime output (automatic line feed)
```
### 0x03 Thoughts
1. Start earlier and make your own plan !
2. Don't forget what you have learnt in class.
3. Review your design with pens and papers (debug from higher level) and don't rely too much on testing.
### 0x04 Acknowledgements
Many thanks to my dear friend @ZhaoYi1031 , his project inspire me a lot.
Also, great work by Pete Sanderson (programming) and Ken Vollmar. Their [MIPS Assembler and Runtime Simulator (Mars)](http://courses.missouristate.edu/KenVollmar/mars/papers.htm) runs smoothly.
