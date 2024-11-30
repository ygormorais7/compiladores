%{
#include <stdio.h>
#include "parser.tab.h"
extern int lineno;
%}

%%
[ \t]+         ; /* Ignora espaços em branco */
\n             { lineno++; return NOVA_LINHA; }
"inteiro"      { return INTEIRO; }
"logico"       { return LOGICO; }
"imprime"      { return IMPRIME; }
"verdadeiro"   { return VERDADEIRO; }
"falso"        { return FALSO; }
"=="           { return IGUAL; }
"!="           { return DIFERENTE; }
">="           { return MAIORIGUAL; }
"<="           { return MENORIGUAL; }
">"            { return MAIOR; }
"<"            { return MENOR; }
"+"            { return MAIS; }
"-"            { return MENOS; }
"*"            { return MULTIPLICA; }
"/"            { return DIVIDE; }
"="            { return ATRIBUICAO; }
"("            { return ABRE_PAREN; }
")"            { return FECHA_PAREN; }
[a-zA-Z_][a-zA-Z0-9_]*  {
    yylval.string = strdup(yytext);
    return ID;
}
[0-9]+         {
    yylval.intval = atoi(yytext);
    return NUM;
}
.              { printf("Caractere inválido na linha %d: %s\n", lineno, yytext); }
%%

int yywrap(void) {
    return 1;
}
