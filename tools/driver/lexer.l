%{
#include <math.h>
#include "parser.hpp"
%}

%option noyywrap

DIGIT    [0-9]
ID       [a-z][a-z0-9]*

%%
{DIGIT}+          { yylval = atof(yytext); return NUM; }
"+"|"-"|"*"|"/"   { return yytext[0]; }
"("|")"           { return yytext[0]; }
[\n]              { return '\n'; }
[ ]               { }
%%
