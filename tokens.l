%{
#include <string>
#include "node.h"
#include "parser.hpp"
#define SAVE_TOKEN yylval.string = new std::string(yytext, yyleng)
#define TOKEN(t) (yylval.token = t)
extern "C" int yywrap() { return 1; }
%}

%%

[ \t\r\n]+               ; // skip whitespace
"int"                    { return TOKEN(TINT); }
"return"                 { return TOKEN(TRETURN); }

";"                      { return TOKEN(TSEMICOLON); }
"="                      { return TOKEN(TEQUAL); }
"=="                     { return TOKEN(TCEQ); }
"!="                     { return TOKEN(TCNE); }
"<"                      { return TOKEN(TCLT); }
"<="                     { return TOKEN(TCLE); }
">"                      { return TOKEN(TCGT); }
">="                     { return TOKEN(TCGE); }
"("                      { return TOKEN(TLPAREN); }
")"                      { return TOKEN(TRPAREN); }
"{"                      { return TOKEN(TLBRACE); }
"}"                      { return TOKEN(TRBRACE); }
","                      { return TOKEN(TCOMMA); }
"+"                      { return TOKEN(TPLUS); }
"-"                      { return TOKEN(TMINUS); }
"*"                      { return TOKEN(TMUL); }
"/"                      { return TOKEN(TDIV); }

[0-9]+\.[0-9]*           { SAVE_TOKEN; return TDOUBLE; }
[0-9]+                   { SAVE_TOKEN; return TINTEGER; }
[a-zA-Z_][a-zA-Z0-9_]*   { SAVE_TOKEN; return TIDENTIFIER; }

.                        { printf("Unknown token: %s\n", yytext); yyterminate(); }
