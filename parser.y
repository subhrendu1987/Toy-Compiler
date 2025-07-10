%{
#include "node.h"
NBlock *programBlock;
extern int yylex();
void yyerror(const char *s) { printf("ERROR: %s\n", s); }
%}

%union {
    Node *node;
    NBlock *block;
    NExpression *expr;
    NStatement *stmt;
    NIdentifier *ident;
    NVariableDeclaration *var_decl;
    std::vector<NVariableDeclaration*> *varvec;
    std::vector<NExpression*> *exprvec;
    std::string *string;
    int token;
}

%token <string> TIDENTIFIER TINTEGER TDOUBLE
%token <token> TCEQ TCNE TCLT TCLE TCGT TCGE TEQUAL
%token <token> TLPAREN TRPAREN TLBRACE TRBRACE TCOMMA TDOT
%token <token> TPLUS TMINUS TMUL TDIV
%token <token> TINT TRETURN TSEMICOLON

%type <ident> ident
%type <expr> numeric expr 
%type <varvec> func_decl_args
%type <exprvec> call_args
%type <block> program stmts block
%type <stmt> stmt func_decl
%type <var_decl> var_decl
%type <token> comparison
%type <ident> type

%left TPLUS TMINUS
%left TMUL TDIV

%start program

%%

program : stmts { programBlock = $1; }
        ;
        
stmts : stmt { $$ = new NBlock(); $$->statements.push_back($1); }
      | stmts stmt { $1->statements.push_back($2); $$ = $1; }
      ;

stmt : var_decl TSEMICOLON
     | func_decl
     | expr TSEMICOLON { $$ = new NExpressionStatement(*$1); }
     ;


block : TLBRACE stmts TRBRACE { $$ = $2; }
      | TLBRACE TRBRACE { $$ = new NBlock(); }
      ;

var_decl : ident ident { $$ = new NVariableDeclaration(*$1, *$2); }
         | type ident TEQUAL expr { $$ = new NVariableDeclaration(*$1, *$2, $4); }
         ;
        
func_decl : type ident TLPAREN func_decl_args TRPAREN block 
            { $$ = new NFunctionDeclaration(*$1, *$2, *$4, *$6); delete $4; }
          ;
    
func_decl_args : /* empty */ { $$ = new VariableList(); }
          | var_decl { $$ = new VariableList(); $$->push_back($1); }
          | func_decl_args TCOMMA var_decl { $1->push_back($3); $$ = $1; }
          ;

ident : TIDENTIFIER { $$ = new NIdentifier(*$1); delete $1; }
      ;

numeric : TINTEGER { $$ = new NInteger(atol($1->c_str())); delete $1; }
        | TDOUBLE { $$ = new NDouble(atof($1->c_str())); delete $1; }
        ;
    
expr : ident TEQUAL expr { $$ = new NAssignment(*$1, *$3); }
     | ident TLPAREN call_args TRPAREN { $$ = new NMethodCall(*$1, *$3); delete $3; }
     | ident { $$ = $1; }
     | numeric
     | expr comparison expr { $$ = new NBinaryOperator(*$1, $2, *$3); }
     | TLPAREN expr TRPAREN { $$ = $2; }
     ;
    
call_args : /* empty */ { $$ = new ExpressionList(); }
          | expr { $$ = new ExpressionList(); $$->push_back($1); }
          | call_args TCOMMA expr { $1->push_back($3); $$ = $1; }
          ;

comparison : TCEQ | TCNE | TCLT | TCLE | TCGT | TCGE 
           | TPLUS | TMINUS | TMUL | TDIV
           ;
    
type : TINT { $$ = new NIdentifier("int"); }
     ;


%%
