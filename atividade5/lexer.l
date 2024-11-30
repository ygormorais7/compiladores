%{
#include "parser.tab.h"  // Inclua o y.tab.h gerado pelo Bison
#include <stdio.h>
%}

%%
"imprime"                  { return IMPRIME; }
"verdadeiro"               { return VERDADEIRO; }
"falso"                    { return FALSO; }

"="                        { return ASSIGN; }

"=="                       { return EQ; }
"!="                       { return NE; }
">="                       { return GE; }
"<="                       { return LE; }
">"                        { return GT; }
"<"                        { return LT; }

"&&"                       { return AND; }
"||"                       { return OR; }
"~"                        { return NOT; }

"+"                        { return PLUS; }
"-"                        { return MINUS; }
"*"                        { return MUL; }
"/"                        { return DIV; }

")"                        { return ')'; }
"("                        { return '('; }

[0-9]+                     { yylval.val = atoi(yytext); return NUMBER; }

[a-zA-Z][a-zA-Z0-9]*       { yylval.name = strdup(yytext); return ID; }

\n			   { return NEWLINE; }

[ \t]                    { /* Ignora espaços em branco */ }

.                          { printf("Token desconhecido: %s\n", yytext); }

%%
