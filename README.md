# TOY COMPILER - A simple compiler for a toy language which is similar to C programming is constructed using FLEX, BISON and LLVM. The toy compier performs everything that a compiler dose, lexical analysis, parsing, semantic analysis, and generating LLVM IR with the help of LLVM'S JIT engine.	

## FEATURES
- Variable declarations and assignments
- Arithmetic Expressions
- Basic code generation using LLVM IR
- Function Definitions and function calls

##PROJECT STRUCTURE
ToyCompiler/
├── lexer.l # Lexical analyzer (Flex)
├── parser.y # Parser with AST hooks (Bison)
├── node.h # AST node definitions
├── codegen.cpp # LLVM code generation logic
├── main.cpp # Compiler driver
├── input.toy # Sample toy-language program
├── README.md # Project documentation


---

##  DEPENDENCIES

Make sure you have the following tools installed:

- **Flex**
- **Bison**
- **LLVM** (10 or later)
- **g++** with C++17 support
- (Optional) **Make**

INSTAL THEM ON UBUNTU/WSL:
```bash
-sudo apt update
-sudo apt install flex bison llvm clang g++ make
```
\\BUILD INSTRUCTIONS\\:
```bash
bison -d -o parser.cpp parser.y
flex -o tokens.cpp lexer.l
g++ -std=c++17 -o parser parser.cpp codegen.cpp main.cpp tokens.cpp `llvm-config --cxxflags --ldflags --system-libs --libs all`
```

RUNNING THE COMPILER:
-Write your program in a file called input.toy
-Then execute :
 ```bash
 ./parser < input.toy
```
-Then you'll get your output

 

