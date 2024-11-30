%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void yyerror(const char *s);
int yylex(void);
int lineno = 1;

// Definição de tipos
#define TYPE_INT 0
#define TYPE_BOOL 1
#define TYPE_ERROR -1

#define VALUE_NORMAL 0
#define VALUE_LOGICAL 1
#define VALUE_RELATIONAL 2

struct {
    char *name;
    int value;
    int type;
    int initialized;
} symtab[100];
int symtab_size = 0;

// Verifica se variável existe
int var_exists(char *name) {
    for(int i = 0; i < symtab_size; i++) {
        if(strcmp(symtab[i].name, name) == 0) {
            return 1;
        }
    }
    return 0;
}

// Obtém o tipo da variável
int get_var_type(char *name) {
    for(int i = 0; i < symtab_size; i++) {
        if(strcmp(symtab[i].name, name) == 0) {
            return symtab[i].type;
        }
    }
    return TYPE_ERROR;
}

void add_symbol(char *name, int type) {
    if (var_exists(name)) {
        fprintf(stderr, "Erro na linha %d: Variável '%s' já declarada\n", lineno, name);
        return;
    }
    symtab[symtab_size].name = strdup(name);
    symtab[symtab_size].type = type;
    symtab[symtab_size].initialized = 0;
    symtab_size++;
    printf("Declaração: %s (%s)\n", name, type == TYPE_INT ? "inteiro" : "logico");
}

int get_value(char *name) {
    for(int i = 0; i < symtab_size; i++) {
        if(strcmp(symtab[i].name, name) == 0) {
            if (!symtab[i].initialized) {
                fprintf(stderr, "Erro na linha %d: Variável '%s' não inicializada\n", lineno, name);
                return 0;
            }
            return symtab[i].value;
        }
    }
    fprintf(stderr, "Erro na linha %d: Variável '%s' não declarada\n", lineno, name);
    return 0;
}

int check_type_compatibility(char *name, int value, int value_type) {
    int var_type = get_var_type(name);
    
    if (var_type == TYPE_ERROR) {
        fprintf(stderr, "Erro na linha %d: Variável '%s' não declarada\n", lineno, name);
        return 0;
    }
    
    // Variável inteira não pode receber valor lógico
    if (var_type == TYPE_INT && value_type == VALUE_LOGICAL) {
        fprintf(stderr, "Erro na linha %d: Não é possível atribuir valor lógico à variável inteira '%s'\n", 
                lineno, name);
        return 0;
    }
    
    // Variável lógica só pode receber valores lógicos ou relacionais
    if (var_type == TYPE_BOOL && value_type == VALUE_NORMAL) {
        fprintf(stderr, "Erro na linha %d: Não é possível atribuir valor inteiro à variável lógica '%s'\n", 
                lineno, name);
        return 0;
    }
    
    return 1;
}

void set_value(char *name, int value, int value_type) {
    if (!check_type_compatibility(name, value, value_type)) {
        return;
    }
    
    for(int i = 0; i < symtab_size; i++) {
        if(strcmp(symtab[i].name, name) == 0) {
            symtab[i].value = value;
            symtab[i].initialized = 1;
            if (symtab[i].type == TYPE_BOOL)
                printf("Atribuição: %s = %s\n", name, value ? "verdadeiro" : "falso");
            else
                printf("Atribuição: %s = %d\n", name, value);
            return;
        }
    }
}

%}

%union {
    int intval;
    char *string;
}

%token INTEIRO LOGICO IMPRIME VERDADEIRO FALSO
%token <string> ID
%token <intval> NUM
%token IGUAL DIFERENTE MAIORIGUAL MENORIGUAL
%token MAIS MENOS MULTIPLICA DIVIDE
%token MAIOR MENOR
%token ATRIBUICAO
%token ABRE_PAREN FECHA_PAREN
%token NOVA_LINHA

%type <intval> exp operator term factor exp2 term2 rel type stmt2
%type <intval> rel_op

%%

program: 
    decl_list stmt_list
    ;

decl_list:
    decl NOVA_LINHA decl_list
    | /* vazio */
    ;

decl:
    type ID    { add_symbol($2, $1); }
    ;

type:
    INTEIRO    { $$ = 0; }
    | LOGICO   { $$ = 1; }
    ;

stmt_list:
    stmt NOVA_LINHA stmt_list
    | stmt
    | /* vazio */
    ;

stmt:
    ID stmt2   { 
        if ($2 != -999999) {
            int value = $2;
            int value_type = VALUE_NORMAL;
            
            // Verifica se é um valor lógico direto (verdadeiro/falso)
            if (value == -2 || value == -3) {
                value = (value == -2) ? 1 : 0;
                value_type = VALUE_LOGICAL;
            }
            // Verifica se é resultado de operação relacional
            else if (value == 0 || value == 1) {
                value_type = VALUE_RELATIONAL;
            }
            
            set_value($1, value, value_type); 
        }
    }
    | print_stmt
    ;

stmt2:
    ATRIBUICAO exp    { $$ = $2; }
    | /* vazio */     { $$ = -999999; }
    ;

print_stmt:
    IMPRIME ABRE_PAREN exp FECHA_PAREN    { 
        int valor = $3;
        int found = 0;
        for(int i = 0; i < symtab_size; i++) {
            if(symtab[i].value == valor && symtab[i].type == TYPE_BOOL) {
                printf("Saída: %s\n", valor ? "verdadeiro" : "falso");
                found = 1;
                break;
            }
        }
        if (!found) {
            printf("Saída: %d\n", valor);
        }
    }
    ;

exp:
    operator rel    { 
        if ($2 == -999999) // Se não houver operação relacional
            $$ = $1;
        else
            $$ = $2;
    }
    ;

operator:
    term exp2    { $$ = $1 + $2; }
    ;

exp2:
    MAIS term exp2     { $$ = $2 + $3; }
    | MENOS term exp2  { $$ = -$2 + $3; }
    | /* vazio */      { $$ = 0; }
    ;

term:
    factor term2    { $$ = $1 * $2; }
    ;

term2:
    MULTIPLICA factor term2    { $$ = $2 * $3; }
    | DIVIDE factor term2      { $$ = $2 ? ($2 / $3) : 0; }
    | /* vazio */             { $$ = 1; }
    ;

factor:
    ABRE_PAREN exp FECHA_PAREN    { $$ = $2; }
    | ID                          { $$ = get_value($1); }
    | NUM                         { $$ = $1; }
    | VERDADEIRO                  { $$ = -2; }  // valor especial para verdadeiro
    | FALSO                       { $$ = -3; }  // valor especial para falso
    ;

rel:
    rel_op operator    { 
        int val1 = $<intval>0; // valor do operador anterior
        int val2 = $2;         // valor do operador atual
        switch($1) {
            case MAIOR: $$ = val1 > val2; break;
            case MENOR: $$ = val1 < val2; break;
            case MAIORIGUAL: $$ = val1 >= val2; break;
            case MENORIGUAL: $$ = val1 <= val2; break;
            case IGUAL: $$ = val1 == val2; break;
            case DIFERENTE: $$ = val1 != val2; break;
            default: $$ = val2;
        }
    }
    | /* vazio */      { $$ = -999999; }
    ;

rel_op:
    MAIOR       { $$ = MAIOR; }
    | MENOR     { $$ = MENOR; }
    | MAIORIGUAL { $$ = MAIORIGUAL; }
    | MENORIGUAL { $$ = MENORIGUAL; }
    | IGUAL     { $$ = IGUAL; }
    | DIFERENTE { $$ = DIFERENTE; }
    ;

%%

void yyerror(const char *s) {
    fprintf(stderr, "Erro na linha %d: %s\n", lineno, s);
}

int main(void) {
    yyparse();
    return 0;
}