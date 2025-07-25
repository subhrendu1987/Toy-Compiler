#ifndef CODEGEN_H
#define CODEGEN_H

#include <stack>
#include <map>
#include <string>
#include "node.h"

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvm/ExecutionEngine/MCJIT.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Support/TargetSelect.h>

using namespace llvm;

class NBlock;

class CodeGenBlock {
public:
    BasicBlock *block;
    std::map<std::string, Value*> locals;
};

class CodeGenContext {
    std::stack<CodeGenBlock *> blocks;
    Function *mainFunction;

public:
    LLVMContext llvmContext;
    IRBuilder<> builder;
    Module *module;

    CodeGenContext() : builder(llvmContext) {
        module = new Module("main", llvmContext);
    }

    void generateCode(NBlock& root);
    GenericValue runCode();
    std::map<std::string, Value*>& locals() { return blocks.top()->locals; }
    BasicBlock *currentBlock() { return blocks.top()->block; }
    void pushBlock(BasicBlock *block) {
        blocks.push(new CodeGenBlock());
        blocks.top()->block = block;
    }
    void popBlock() {
        CodeGenBlock *top = blocks.top();
        blocks.pop();
        delete top;
    }
};

#endif
