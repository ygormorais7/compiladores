#ifndef PARSER_H
#define PARSER_H

#include <stdbool.h>
#include "ast.h"
#include "token.h"

typedef struct {
    Token **tokens;
    int current_token;
    int total_tokens;
} Parser;

// Funções básicas do parser
Parser* create_parser(Token **tokens);
void free_parser(Parser *parser);

// Funções de manipulação de tokens
Token* get_current_token(Parser *parser);
void advance_token(Parser *parser);
bool match_token(Parser *parser, const char *expected_type);

// Funções de parsing
ASTNode* parse_program(Parser *parser);
ASTNode* parse_decl_list(Parser *parser);
ASTNode* parse_stmt_list(Parser *parser);
ASTNode* parse_decl(Parser *parser);
ASTNode* parse_stmt(Parser *parser);
ASTNode* parse_stmt2(Parser *parser);
ASTNode* parse_print_stmt(Parser *parser);
ASTNode* parse_exp(Parser *parser);
ASTNode* parse_operator(Parser *parser);
ASTNode* parse_exp2(Parser *parser);
ASTNode* parse_term(Parser *parser);
ASTNode* parse_term2(Parser *parser);
ASTNode* parse_factor(Parser *parser);
ASTNode* parse_rel(Parser *parser);

// Função de erro
void print_error(const char *message, Token *token);

#endif