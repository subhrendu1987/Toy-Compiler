#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <llvm/IR/Value.h>

class CodeGenContext;
class NExpression;
class NStatement;
class NVariableDeclaration;

// Typedefs required by the parser.y rules
typedef std::vector<NExpression*> ExpressionList;
typedef std::vector<NStatement*> StatementList;
typedef std::vector<NVariableDeclaration*> VariableList;


// ✅ Only one definition of Node
class Node {
public:
    virtual ~Node() = default;
    virtual llvm::Value* codeGen(CodeGenContext& context) = 0;
};

// Base Expression node
class NExpression : public Node {
public:
    virtual ~NExpression() = default;
};

// Base Statement node
class NStatement : public Node {
public:
    virtual ~NStatement() = default;
};

// Integer literal
class NInteger : public NExpression {
public:
    long long value;
    NInteger(long long value) : value(value) {}
    llvm::Value* codeGen(CodeGenContext& context) override;
};

// Double literal
class NDouble : public NExpression {
public:
    double value;
    NDouble(double value) : value(value) {}
    llvm::Value* codeGen(CodeGenContext& context) override;
};

// Identifier (variable name)
class NIdentifier : public NExpression {
public:
    std::string name;
    NIdentifier(const std::string& name) : name(name) {}
    llvm::Value* codeGen(CodeGenContext& context) override;
};

// Method/function call
class NMethodCall : public NExpression {
public:
    NIdentifier id;
    std::vector<NExpression*> arguments;
    NMethodCall(const NIdentifier& id, const std::vector<NExpression*>& args)
        : id(id), arguments(args) {}
    llvm::Value* codeGen(CodeGenContext& context) override;
};

// Binary operator expression (e.g., +, *, etc.)
class NBinaryOperator : public NExpression {
public:
    int op;
    NExpression& lhs;
    NExpression& rhs;
    NBinaryOperator(NExpression& lhs, int op, NExpression& rhs)
        : lhs(lhs), rhs(rhs), op(op) {}
    llvm::Value* codeGen(CodeGenContext& context) override;
};

// Assignment expression (e.g., a = b + c)
class NAssignment : public NExpression {
public:
    NIdentifier& lhs;
    NExpression& rhs;
    NAssignment(NIdentifier& lhs, NExpression& rhs)
        : lhs(lhs), rhs(rhs) {}
    llvm::Value* codeGen(CodeGenContext& context) override;
};

// A block of statements (i.e., compound statement)
class NBlock : public NExpression {
public:
    std::vector<NStatement*> statements;
    NBlock() = default;
    llvm::Value* codeGen(CodeGenContext& context) override;
};

// Expression used as a statement
class NExpressionStatement : public NStatement {
public:
    NExpression& expression;
    NExpressionStatement(NExpression& expression)
        : expression(expression) {}
    llvm::Value* codeGen(CodeGenContext& context) override;
};

// Variable declaration (e.g., int x = 5)
class NVariableDeclaration : public NStatement {
public:
    NIdentifier type;
    NIdentifier id;
    NExpression* assignmentExpr;

    NVariableDeclaration(NIdentifier type, NIdentifier id, NExpression* expr = nullptr)
        : type(type), id(id), assignmentExpr(expr) {}
    llvm::Value* codeGen(CodeGenContext& context) override;
};

// Function declaration (e.g., int f(int x) { ... })
class NFunctionDeclaration : public NStatement {
public:
    NIdentifier type;
    NIdentifier id;
    std::vector<NVariableDeclaration*> arguments;
    NBlock& block;

    NFunctionDeclaration(NIdentifier type, NIdentifier id,
                         const std::vector<NVariableDeclaration*>& args,
                         NBlock& block)
        : type(type), id(id), arguments(args), block(block) {}
    llvm::Value* codeGen(CodeGenContext& context) override;
};
