#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#include "../include/token.h"
#include "../include/reader.h"
#include "../include/symbolTable.h"

Token reserved_tokens[] = {
    {"PRINT", "imprime", "TK_PRINT"},
    {"TRUE", "verdadeiro", "TK_BOOL"},
    {"FALSE", "falso", "TK_BOOL"},
    {"INT", "inteiro", "TK_TYPE"},
    {"BOOL", "logico", "TK_TYPE"}
};

int reserved_count = sizeof(reserved_tokens) / sizeof(reserved_tokens[0]);

SymbolTable *symbol_table;

void init_lexer() {
    symbol_table = create_symbol_table();
}

bool is_assign(char chr) {
    return chr == '=';
}

bool is_arit_operator(char chr){
    if (chr == '+' || chr == '-' || chr == '*' || chr == '/'){
        return true;
    }
    return false;
}

bool is_rel_operator(const char *str){
    if (str[0] == '>' || str[0] == '<' || (str[0] == '=' && str[1] == '=') || (str[0] == '!' && str[1] == '=') || (str[0] == '>' && str[1] == '=') || (str[0] == '<' && str[1] == '=') ){
        return true;
    }
    return false;
}

bool is_operator(char chr, const char *nextChr){
    char str[3] = {chr, nextChr ? *nextChr : '\0', '\0'};
    if (is_rel_operator(str) || is_arit_operator(chr) || is_assign(chr)) {
        return true;
    }
    return false;
}

bool is_delimiter(char chr){
    if (chr == ' ' || chr == ',' || chr == ';' || chr == ')' || chr == '(' || chr == '\n' || chr == '\0'){
        return true;
    }
    return false;
}

bool is_not_letter(char chr, const char *nextChr) {
    if(is_delimiter(chr) || is_operator(chr, nextChr)){
        return true;
    }
    return false;
}

bool is_integer(const char *str) {
    if (str[0] == '\0') {
        return false;
    }
    for (int i = 0; str[i] != '\0'; i++) {
        if (!isdigit(str[i])) {
            return false;
        }
    }
    return true;
}

bool is_reserved(const char *subStr) {
    for (int i = 0; i < reserved_count; i++) {
        if (strcmp(subStr, reserved_tokens[i].valor) == 0) {
            return true;
        }
    }
    return false;
}

bool is_identifier(const char *str) {
    if (!isalpha(str[0]) && str[0] != '_') {
        return false;
    }
    for (int i = 1; str[i] != '\0'; i++) {
        if (!isalnum(str[i]) && str[i] != '_') {
            return false;
        }
    }
    return true;
}

bool is_not_valid(char chr, const char *nextChr) {
    return !isalpha(chr) && !isdigit(chr) && !is_delimiter(chr) && !is_operator(chr, nextChr);
}

char* extract_substr(const char *text, int left, int right){
    int sub_len = right - left + 1;
    char *subStr = (char *)malloc((sub_len + 1) * sizeof(char));

    strncpy(subStr, text + left, sub_len);
    subStr[sub_len] = '\0';

    return subStr;
}

// FLUXO INICIAL DO ANALISADOR LÉXICO
Token** lexical_analyzer(const char *text) {
    int left = 0, right = 0;
    int len = strlen(text);
    Token **tokens = (Token **)malloc(512 * sizeof(Token *));
    int tokens_count = 0;

    // Inicializar todos os ponteiros como NULL
    for (int i = 0; i < 512; i++) {
        tokens[i] = NULL;
    }

    // Inicializar tabela de símbolos
    init_lexer();

    while (right <= len && left <= right) {
        if (!is_not_letter(text[right], &text[right+1])) {
            right++;
        }

        if ((is_not_letter(text[right], &text[right+1]) || right == len) && left < right) {
            char *subStr = extract_substr(text, left, right-1);
            if (!subStr) {
                fprintf(stderr, "Erro de alocação de memória\n");
                // Limpar tokens já alocados
                for (int i = 0; i < tokens_count; i++) {
                    free_token(tokens[i]);
                }
                free(tokens);
                return NULL;
            }

            if (is_reserved(subStr)) {
                for (int i = 0; i < reserved_count; i++) {
                    if (strcmp(subStr, reserved_tokens[i].valor) == 0) {
                        tokens[tokens_count] = create_token(
                            reserved_tokens[i].nome,
                            reserved_tokens[i].valor,
                            reserved_tokens[i].tipo
                        );
                        if (!tokens[tokens_count]) {
                            fprintf(stderr, "Erro ao criar token\n");
                            free(subStr);
                            // Limpar tokens
                            for (int j = 0; j < tokens_count; j++) {
                                free_token(tokens[j]);
                            }
                            free(tokens);
                            return NULL;
                        }
                        tokens_count++;
                        printf("TOKEN: <%s, %s>\n", 
                               reserved_tokens[i].tipo,
                               reserved_tokens[i].valor);
                        break;
                    }
                }
            } else if (is_identifier(subStr)) {
                tokens[tokens_count] = create_token("ID", subStr, "TK_ID");
                if (!tokens[tokens_count]) {
                    fprintf(stderr, "Erro ao criar token\n");
                    free(subStr);
                    // Limpar tokens
                    for (int i = 0; i < tokens_count; i++) {
                        free_token(tokens[i]);
                    }
                    free(tokens);
                    return NULL;
                }
                printf("TOKEN: <TK_ID, %s>\n", subStr);
                
                // Adicionar à tabela de símbolos
                if (find_symbol(symbol_table, subStr) == NULL) {
                    add_symbol(symbol_table, subStr, "unknown", false);
                }
                tokens_count++;
            } else if (is_integer(subStr)) {
                tokens[tokens_count] = create_token("INT", subStr, "TK_NUM");
                if (!tokens[tokens_count]) {
                    fprintf(stderr, "Erro ao criar token\n");
                    free(subStr);
                    // Limpar tokens
                    for (int i = 0; i < tokens_count; i++) {
                        free_token(tokens[i]);
                    }
                    free(tokens);
                    return NULL;
                }
                printf("TOKEN: <TK_NUM, %s>\n", subStr);
                tokens_count++;
            }

            free(subStr);
            left = right;
        }

        // Tratamento de operadores e delimitadores
        if (is_operator(text[right], &text[right+1])) {
            char subStr[3] = {text[right], text[right + 1], '\0'};
            
            if (is_rel_operator(subStr)) {
                tokens[tokens_count++] = create_token("RELOP", subStr, "TK_RELOP");
                printf("TOKEN: <TK_RELOP, %s>\n", subStr);
                right += 2;
            } else {
                char tempStr[2] = {text[right], '\0'};
                if (is_arit_operator(text[right])) {
                    tokens[tokens_count++] = create_token("ARIOP", tempStr, "TK_ARIOP");
                    printf("TOKEN: <TK_ARIOP, %s>\n", tempStr);
                } else if (is_assign(text[right])) {
                    tokens[tokens_count++] = create_token("ASSIGNOP", tempStr, "TK_ASSIGNOP");
                    printf("TOKEN: <TK_ASSIGNOP, %s>\n", tempStr);
                }
                right++;
            }
            left = right;
            continue;
        }

        if (is_delimiter(text[right])) {
            if (text[right] == '(') {
                tokens[tokens_count++] = create_token("LPAREN", "(", "TK_LPAREN");
                printf("TOKEN: <TK_LPAREN, (>\n");
            } else if (text[right] == ')') {
                tokens[tokens_count++] = create_token("RPAREN", ")", "TK_RPAREN");
                printf("TOKEN: <TK_RPAREN, )>\n");
            } else if (text[right] == '\n') {
                tokens[tokens_count++] = create_token("LINEBREAK", "\\n", "TK_BREAK");
                printf("TOKEN: <TK_BREAK, \\n>\n");
            } else if (text[right] == '\0') {
                tokens[tokens_count++] = create_token("ENDOFSTRING", "\\0", "TK_BREAK");
                printf("TOKEN: <TK_BREAK, \\0>\n");
            }
            right++;
            left = right;
            continue;
        }

        if (is_not_valid(text[right], &text[right + 1])) {
            printf("ERROR: Caractere não reconhecido '%c'\n", text[right]);
            right++;
            left = right;
        }
    }

    // Garantir que o último token seja NULL
    tokens[tokens_count] = NULL;
    
    return tokens;
}
