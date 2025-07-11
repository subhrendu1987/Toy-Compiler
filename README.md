# TOY COMPILER 
- A simple compiler for a toy language which is similar to C programming is constructed using FLEX, BISON and LLVM. The toy compier performs everything that a compiler dose, lexical analysis, parsing, semantic analysis, and generating LLVM IR with the help of LLVM'S JIT engine.	

## FEATURES
- Variable declarations and assignments
- Arithmetic Expressions
- Basic code generation using LLVM IR
- Function Definitions and function calls

## Environment Preparations
###  DEPENDENCIES
- **Flex**
- **Bison**
- **LLVM** (10 or later)
- **g++** with C++17 support
- (Optional) **Make**

### Installation commands
```bash
sudo apt update
sudo apt install flex bison llvm clang g++ make -y
```

## Files
```
ToyCompiler/
1.  lexer.l # Lexical analyzer (Flex)
2.  parser.y # Parser with AST hooks (Bison)
3.  node.h # AST node definitions
4.  codegen.cpp # LLVM code generation logic
5.  main.cpp # Compiler driver
6.  input.toy # Sample toy-language program
7.  README.md # Project documentation
8.  Instructions.pdf # Original blog-post
```

---
## Build compiler
```bash
bison -d -o parser.cpp parser.y
#flex -o tokens.cpp lexer.l
lex -o tokens.cpp tokens.l
g++ -std=c++17 -o parser parser.cpp codegen.cpp main.cpp tokens.cpp `llvm-config --cxxflags --ldflags --system-libs --libs all`
```



RUNNING THE COMPILER:
-Write your program in a file called input.toy
-Then execute :
 ```bash
 ./parser < input.toy
```
-Then you'll get your output

 
(https://gnuu.org/2009/09/18/writing-your-own-toy-compiler/)

