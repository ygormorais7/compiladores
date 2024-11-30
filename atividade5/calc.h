#ifndef CALC_H
#define CALC_H

#include "parser.tab.h"  // Inclua o arquivo gerado pelo Bison para as definições de tokens e YYSTYPE

// Declarações de funções
void yyerror(const char *s);  // Função de erro usada pelo Bison

// Variáveis globais (se necessário)
// extern YYSTYPE yylval;  // Exemplo, caso você precise acessar yylval diretamente em outros arquivos

#endif // CALC_H
