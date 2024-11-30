#include "../include/token.h"

Token* create_token(const char *nome, const char *valor, const char *tipo) {
    Token *token = (Token *)malloc(sizeof(Token));
    token->nome = strdup(nome);
    token->valor = strdup(valor);
    token->tipo = strdup(tipo);
    return token;
}

void free_token(Token *token) {
    if (token) {
        free(token->nome);
        free(token->valor);
        free(token->tipo);
        free(token);
    }
}
