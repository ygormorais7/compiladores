%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "calc.h"  // Inclua o arquivo de cabeçalho calc.h

#define MAX_VARS 100

// Estrutura para armazenar variáveis
typedef struct {
    char name[50];
    int value;
} Variable;

Variable vars[MAX_VARS];
int var_count = 0;

// Função para buscar o valor de uma variável
int get_var_value(const char *name) {
    for (int i = 0; i < var_count; i++) {
        if (strcmp(vars[i].name, name) == 0) {
            return vars[i].value;
        }
    }
    printf("Variável não encontrada: %s\n", name);
    return 0; // Retorna 0 se a variável não for encontrada
}

// Função para definir o valor de uma variável
void set_var_value(const char *name, int value) {
    for (int i = 0; i < var_count; i++) {
        if (strcmp(vars[i].name, name) == 0) {
            vars[i].value = value;
            return;
        }
    }
    // Se a variável não foi encontrada, cria uma nova
    strcpy(vars[var_count].name, name);
    vars[var_count].value = value;
    var_count++;
}

// Declaração da função de erro
void yyerror(const char *s) {
    fprintf(stderr, "Erro: %s\n", s);
}

// Declaração da função yylex
int yylex();
%}

%union {
    int val;        // Para armazenar números inteiros
    char *name;     // Para armazenar identificadores (nomes de variáveis)
}

%token <val> NUMBER
%token <name> ID
%token ASSIGN IMPRIME VERDADEIRO FALSO
%token EQ NE GT GE LT LE
%token AND OR NOT PLUS MINUS MUL DIV
%token NEWLINE

%type <val> expressao termo fator
%type <name> comando atribuicao imprime comparacao

%left '+' '-'
%left '*' '/'
%left EQ NE GT GE LT LE

%%

programa:
    programa comando NEWLINE   // Cada comando termina com NEWLINE
    | /* vazio */
    ;

comando:
    atribuicao
    | imprime
    | comparacao
    ;

atribuicao:
    ID ASSIGN expressao {
        set_var_value($1, $3);  // Armazena o valor da variável
        printf("Atribuicao: %s = %d\n", $1, $3);
    }
    ;

imprime:
    IMPRIME '(' expressao ')' {
        printf("Imprime: %d\n", $3);
    }
    ;

comparacao:
    ID ASSIGN expressao EQ expressao {
        printf("Comparacao: %s == %d resulta em %d\n", $1, $3, get_var_value($1) == $5);
    }
    ;

expressao:
    termo PLUS expressao { $$ = $1 + $3; }
    | termo MINUS expressao { $$ = $1 - $3; }
    | termo
    ;

termo:
    fator MUL termo { $$ = $1 * $3; }
    | fator DIV termo { $$ = $1 / $3; }
    | fator
    ;

fator:
    '(' expressao ')' { $$ = $2; }
    | ID { $$ = get_var_value($1); }
    | NUMBER { $$ = $1; }
    | VERDADEIRO { $$ = 1; }
    | FALSO { $$ = 0; }
    ;

%%

// Função principal
int main() {
    printf("Iniciando o parser...\n");
    return yyparse();
}
