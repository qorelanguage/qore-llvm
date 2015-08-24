%{
#include <math.h>
#include "parser.hpp"

extern bool lexNlIsEof;
%}

%option noyywrap

%%
print               { return KW_PRINT; }
[0-9]+              { yylval.number = atoi(yytext); return NUMBER; }
[a-z][a-z0-9]*      { yylval.string = strdup(yytext); return IDENTIFIER; }
\"[^\"]*\"          { yylval.string = strdup(yytext + 1); yylval.string[strlen(yylval.string)-1] = 0; return STRING; }
"="|";"             { return yytext[0]; }
[ \t\r]             { }
[\n]                { if (lexNlIsEof) return -1; }
%%
