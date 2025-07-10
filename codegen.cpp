#include "node.h"
#include "codegen.h"
#include "parser.hpp"

#include <iostream>
#include <vector>

using namespace llvm;
using namespace std;

// Return LLVM type based on identifier
static Type* typeOf(CodeGenContext& context, const NIdentifier& type) {
    if (type.name == "int") {
        return Type::getInt64Ty(context.llvmContext);
    } else if (type.name == "double") {
        return Type::getDoubleTy(context.llvmContext);
    }
    return Type::getVoidTy(context.llvmContext);
}

// Generate LLVM IR from the AST
void CodeGenContext::generateCode(NBlock& root) {
    std::cout << "Generating code..." << std::endl;

    // Define: void main()
    FunctionType* ftype = FunctionType::get(Type::getVoidTy(llvmContext), false);
    mainFunction = Function::Create(ftype, Function::InternalLinkage, "main", module);
    BasicBlock* bblock = BasicBlock::Create(llvmContext, "entry", mainFunction);

    pushBlock(bblock);
    builder.SetInsertPoint(bblock);  // Important for IRBuilder

    root.codeGen(*this);  // Generate body
    builder.CreateRetVoid();

    popBlock();

    std::cout << "Code is generated." << std::endl;
    module->print(outs(), nullptr);
}

// Execute the compiled LLVM IR
GenericValue CodeGenContext::runCode() {
    std::cout << "Running code..." << std::endl;

    InitializeNativeTarget();
    InitializeNativeTargetAsmPrinter();
    InitializeNativeTargetAsmParser();

    std::string errStr;
    auto owner = std::unique_ptr<Module>(module);
    ExecutionEngine* engine = EngineBuilder(std::move(owner))
        .setErrorStr(&errStr)
        .setEngineKind(EngineKind::JIT)
        .create();

    if (!engine) {
        std::cerr << "Failed to create ExecutionEngine: " << errStr << std::endl;
        exit(1);
    }

    std::vector<GenericValue> noargs;
    GenericValue result = engine->runFunction(mainFunction, noargs);
    std::cout << "Code was run." << std::endl;
    return result;
}

// ---------- Node CodeGen Implementations ---------- //

Value* NInteger::codeGen(CodeGenContext& context) {
    return ConstantInt::get(Type::getInt64Ty(context.llvmContext), value, true);
}

Value* NDouble::codeGen(CodeGenContext& context) {
    return ConstantFP::get(Type::getDoubleTy(context.llvmContext), value);
}

Value* NIdentifier::codeGen(CodeGenContext& context) {
    if (context.locals().find(name) == context.locals().end()) {
        std::cerr << "Undeclared variable: " << name << std::endl;
        return nullptr;
    }
    Value* ptr = context.locals()[name];
    llvm::Type* valueType = llvm::cast<AllocaInst>(ptr)->getAllocatedType();
    return context.builder.CreateLoad(valueType, ptr, name.c_str());
}

Value* NMethodCall::codeGen(CodeGenContext& context) {
    Function* function = context.module->getFunction(id.name);
    if (!function) {
        std::cerr << "No such function: " << id.name << std::endl;
        return nullptr;
    }

    std::vector<Value*> args;
    for (auto& arg : arguments) {
        args.push_back(arg->codeGen(context));
    }

    return context.builder.CreateCall(function, args, "calltmp");
}

Value* NBinaryOperator::codeGen(CodeGenContext& context) {
    Value* L = lhs.codeGen(context);
    Value* R = rhs.codeGen(context);
    if (!L || !R) return nullptr;

    switch (op) {
        case TPLUS:  return context.builder.CreateAdd(L, R, "addtmp");
        case TMINUS: return context.builder.CreateSub(L, R, "subtmp");
        case TMUL:   return context.builder.CreateMul(L, R, "multmp");
        case TDIV:   return context.builder.CreateSDiv(L, R, "divtmp");
        default:
            std::cerr << "Unknown binary operator: " << op << std::endl;
            return nullptr;
    }
}

Value* NAssignment::codeGen(CodeGenContext& context) {
    Value* value = rhs.codeGen(context);
    if (!value) return nullptr;

    if (context.locals().find(lhs.name) == context.locals().end()) {
        std::cerr << "Undeclared variable: " << lhs.name << std::endl;
        return nullptr;
    }

    context.builder.CreateStore(value, context.locals()[lhs.name]);
    return value;
}

Value* NBlock::codeGen(CodeGenContext& context) {
    Value* last = nullptr;
    for (auto& stmt : statements) {
        last = stmt->codeGen(context);
    }
    return last;
}

Value* NExpressionStatement::codeGen(CodeGenContext& context) {
    return expression.codeGen(context);
}

Value* NVariableDeclaration::codeGen(CodeGenContext& context) {
    Type* varType = typeOf(context, type);
    AllocaInst* alloc = context.builder.CreateAlloca(varType, nullptr, id.name);
    context.locals()[id.name] = alloc;

    if (assignmentExpr != nullptr) {
        NAssignment assign(id, *assignmentExpr);
        assign.codeGen(context);
    }

    return alloc;
}

Value* NFunctionDeclaration::codeGen(CodeGenContext& context) {
    std::vector<Type*> argTypes;
    for (const auto& arg : arguments) {
        argTypes.push_back(typeOf(context, arg->type));
    }

    FunctionType* ftype = FunctionType::get(typeOf(context, type), argTypes, false);
    Function* function = Function::Create(ftype, Function::InternalLinkage, id.name, context.module);
    BasicBlock* bblock = BasicBlock::Create(context.llvmContext, "entry", function);

    context.pushBlock(bblock);
    context.builder.SetInsertPoint(bblock);

    unsigned idx = 0;
    for (auto& arg : function->args()) {
        arg.setName(arguments[idx]->id.name);
        AllocaInst* alloc = context.builder.CreateAlloca(arg.getType(), nullptr, arg.getName());
        context.builder.CreateStore(&arg, alloc);
        context.locals()[arg.getName().str()] = alloc;
        idx++;
    }

    block.codeGen(context);
    context.builder.CreateRetVoid();
    context.popBlock();

    return function;
}
