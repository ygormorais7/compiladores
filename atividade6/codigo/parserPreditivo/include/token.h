#ifndef TOKEN_H
#define TOKEN_H

#include <stdlib.h>
#include <string.h>

typedef struct {
    char *nome;
    char *valor;
    char *tipo;
} Token;

Token* create_token(const char *nome, const char *valor, const char *tipo);
void free_token(Token *token);

#endif