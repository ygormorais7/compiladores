#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/parser.h"
#include "../include/ast.h"
#include "../include/token.h"

ASTNode* parse_stmt(Parser *parser);
ASTNode* parse_stmt2(Parser *parser);
ASTNode* parse_print_stmt(Parser *parser);
ASTNode* parse_exp(Parser *parser);
ASTNode* parse_operator(Parser *parser);
ASTNode* parse_exp2(Parser *parser);
ASTNode* parse_term(Parser *parser);
ASTNode* parse_term2(Parser *parser);
ASTNode* parse_factor(Parser *parser);

Parser* create_parser(Token **tokens) {
    Parser *parser = malloc(sizeof(Parser));
    if (!parser) return NULL;
    
    parser->tokens = tokens;
    parser->current_token = 0;
    parser->total_tokens = 0;
    
    // Conta total de tokens
    while (tokens[parser->total_tokens] != NULL) {
        parser->total_tokens++;
    }
    
    return parser;
}

void free_parser(Parser *parser) {
    if (parser) {
        // Não libera os tokens aqui, pois eles são gerenciados pelo lexer
        free(parser);
    }
}

void print_error(const char *message, Token *token) {
    if (token) {
        fprintf(stderr, "Erro sintático: %s próximo a '%s'\n", 
                message, token->valor);
    } else {
        fprintf(stderr, "Erro sintático: %s no fim do arquivo\n", 
                message);
    }
}

// Funções de manipulação de tokens
Token* get_current_token(Parser *parser) {
    if (parser && parser->current_token < parser->total_tokens) {
        return parser->tokens[parser->current_token];
    }
    return NULL;
}

void advance_token(Parser *parser) {
    if (parser && parser->current_token < parser->total_tokens) {
        parser->current_token++;
    }
}

// Funções auxiliares
bool match_token(Parser *parser, const char *expected_type) {
    Token *current = get_current_token(parser);
    if (current && strcmp(current->tipo, expected_type) == 0) {
        advance_token(parser);
        return true;
    }
    return false;
}

// Implementação das funções de parsing
ASTNode* parse_program(Parser *parser) {
    ASTNode *program = create_ast_node(NODE_PROGRAM, "program");
    if (!program) return NULL;

    ASTNode *decl_list = parse_decl_list(parser);
    ASTNode *stmt_list = parse_stmt_list(parser);

    if (decl_list) add_child(program, decl_list);
    if (stmt_list) add_child(program, stmt_list);

    return program;
}

ASTNode* parse_decl_list(Parser *parser) {
    Token *current = get_current_token(parser);
    
    if (current && strcmp(current->tipo, "TK_TYPE") == 0) {
        ASTNode *decl_list = create_ast_node(NODE_DECL, "decl_list");
        if (!decl_list) return NULL;

        ASTNode *decl = parse_decl(parser);
        if (!decl) {
            free_ast(decl_list);
            return NULL;
        }
        add_child(decl_list, decl);

        // Verifica quebra de linha
        if (!match_token(parser, "TK_BREAK")) {
            print_error("Expected newline after declaration", get_current_token(parser));
            free_ast(decl_list);
            return NULL;
        }

        // Recursivamente parse o resto da lista
        ASTNode *next_decl = parse_decl_list(parser);
        if (next_decl) add_child(decl_list, next_decl);

        return decl_list;
    }
    return NULL; // ε production
}

ASTNode* parse_decl(Parser *parser) {
    Token *current = get_current_token(parser);
    if (!current || strcmp(current->tipo, "TK_TYPE") != 0) {
        print_error("Expected type declaration", current);
        return NULL;
    }

    ASTNode *decl = create_ast_node(NODE_DECL, current->valor);
    advance_token(parser);

    // Espera um identificador
    current = get_current_token(parser);
    if (!current || strcmp(current->tipo, "TK_ID") != 0) {
        print_error("Expected identifier", current);
        free_ast(decl);
        return NULL;
    }

    ASTNode *id = create_ast_node(NODE_ID, current->valor);
    add_child(decl, id);
    advance_token(parser);

    return decl;
}

ASTNode* parse_stmt_list(Parser *parser) {
    Token *current = get_current_token(parser);
    
    if (current && (strcmp(current->tipo, "TK_ID") == 0 || 
                   strcmp(current->tipo, "TK_PRINT") == 0)) {
        ASTNode *stmt_list = create_ast_node(NODE_STMT, "stmt_list");
        
        ASTNode *stmt = parse_stmt(parser);
        if (!stmt) {
            free_ast(stmt_list);
            return NULL;
        }
        add_child(stmt_list, stmt);

        // Verifica quebra de linha
        if (!match_token(parser, "TK_BREAK")) {
            print_error("Expected newline after statement", get_current_token(parser));
            free_ast(stmt_list);
            return NULL;
        }

        // Recursivamente parse o resto da lista
        ASTNode *next_stmt = parse_stmt_list(parser);
        if (next_stmt) add_child(stmt_list, next_stmt);

        return stmt_list;
    }
    return NULL; // ε production
}

ASTNode* parse_stmt(Parser *parser) {
    Token *current = get_current_token(parser);
    if (!current) return NULL;

    if (strcmp(current->tipo, "TK_ID") == 0) {
        ASTNode *stmt = create_ast_node(NODE_STMT, "statement");
        ASTNode *id = create_ast_node(NODE_ID, current->valor);
        add_child(stmt, id);
        advance_token(parser);

        ASTNode *stmt2 = parse_stmt2(parser);
        if (stmt2) add_child(stmt, stmt2);

        return stmt;
    } else if (strcmp(current->tipo, "TK_PRINT") == 0) {
        return parse_print_stmt(parser);
    }

    print_error("Expected statement", current);
    return NULL;
}

ASTNode* parse_stmt2(Parser *parser) {
    Token *current = get_current_token(parser);
    
    if (current && strcmp(current->valor, "=") == 0) {
        advance_token(parser);
        ASTNode *assign = create_ast_node(NODE_ASSIGN, "=");
        
        ASTNode *exp = parse_exp(parser);
        if (!exp) {
            free_ast(assign);
            return NULL;
        }
        add_child(assign, exp);
        
        return assign;
    }
    return NULL; // ε production
}

ASTNode* parse_print_stmt(Parser *parser) {
    if (!match_token(parser, "TK_PRINT")) {
        print_error("Expected 'imprime'", get_current_token(parser));
        return NULL;
    }

    ASTNode *print = create_ast_node(NODE_PRINT, "imprime");

    if (!match_token(parser, "TK_LPAREN")) {
        print_error("Expected '('", get_current_token(parser));
        free_ast(print);
        return NULL;
    }

    ASTNode *exp = parse_exp(parser);
    if (!exp) {
        free_ast(print);
        return NULL;
    }
    add_child(print, exp);

    if (!match_token(parser, "TK_RPAREN")) {
        print_error("Expected ')'", get_current_token(parser));
        free_ast(print);
        return NULL;
    }

    return print;
}

ASTNode* parse_exp(Parser *parser) {
    ASTNode *exp = create_ast_node(NODE_EXPR, "expression");
    
    ASTNode *operator = parse_operator(parser);
    if (!operator) {
        free_ast(exp);
        return NULL;
    }
    add_child(exp, operator);

    ASTNode *rel = parse_rel(parser);
    if (rel) add_child(exp, rel);

    return exp;
}

ASTNode* parse_operator(Parser *parser) {
    ASTNode *term = parse_term(parser);
    if (!term) return NULL;

    ASTNode *operator = create_ast_node(NODE_OPERATOR, "operator");
    add_child(operator, term);

    ASTNode *exp2 = parse_exp2(parser);
    if (exp2) add_child(operator, exp2);

    return operator;
}

ASTNode* parse_exp2(Parser *parser) {
    Token *current = get_current_token(parser);
    
    if (current && (strcmp(current->valor, "+") == 0 || 
                   strcmp(current->valor, "-") == 0)) {
        ASTNode *exp2 = create_ast_node(NODE_OPERATOR, current->valor);
        advance_token(parser);

        ASTNode *term = parse_term(parser);
        if (!term) {
            free_ast(exp2);
            return NULL;
        }
        add_child(exp2, term);

        ASTNode *next_exp2 = parse_exp2(parser);
        if (next_exp2) add_child(exp2, next_exp2);

        return exp2;
    }
    return NULL; // ε production
}

ASTNode* parse_term(Parser *parser) {
    ASTNode *factor = parse_factor(parser);
    if (!factor) return NULL;

    ASTNode *term = create_ast_node(NODE_TERM, "term");
    add_child(term, factor);

    ASTNode *term2 = parse_term2(parser);
    if (term2) add_child(term, term2);

    return term;
}

ASTNode* parse_term2(Parser *parser) {
    Token *current = get_current_token(parser);
    
    if (current && (strcmp(current->valor, "*") == 0 || 
                   strcmp(current->valor, "/") == 0)) {
        ASTNode *term2 = create_ast_node(NODE_OPERATOR, current->valor);
        advance_token(parser);

        ASTNode *factor = parse_factor(parser);
        if (!factor) {
            free_ast(term2);
            return NULL;
        }
        add_child(term2, factor);

        ASTNode *next_term2 = parse_term2(parser);
        if (next_term2) add_child(term2, next_term2);

        return term2;
    }
    return NULL; // ε production
}

ASTNode* parse_factor(Parser *parser) {
    Token *current = get_current_token(parser);
    if (!current) return NULL;

    if (strcmp(current->tipo, "TK_LPAREN") == 0) {
        advance_token(parser);
        ASTNode *exp = parse_exp(parser);
        
        if (!match_token(parser, "TK_RPAREN")) {
            print_error("Expected ')'", get_current_token(parser));
            free_ast(exp);
            return NULL;
        }
        return exp;
    } 
    else if (strcmp(current->tipo, "TK_ID") == 0) {
        ASTNode *id = create_ast_node(NODE_ID, current->valor);
        advance_token(parser);
        return id;
    }
    else if (strcmp(current->tipo, "TK_NUM") == 0) {
        ASTNode *num = create_ast_node(NODE_NUM, current->valor);
        advance_token(parser);
        return num;
    }
    else if (strcmp(current->tipo, "TK_BOOL") == 0) {
        ASTNode *bool_node = create_ast_node(NODE_BOOL, current->valor);
        advance_token(parser);
        return bool_node;
    }

    print_error("Expected factor", current);
    return NULL;
}

ASTNode* parse_rel(Parser *parser) {
    Token *current = get_current_token(parser);
    
    if (current && (strcmp(current->valor, ">") == 0 || 
                   strcmp(current->valor, "<") == 0 ||
                   strcmp(current->valor, ">=") == 0 ||
                   strcmp(current->valor, "<=") == 0 ||
                   strcmp(current->valor, "==") == 0 ||
                   strcmp(current->valor, "!=") == 0)) {
        
        ASTNode *rel = create_ast_node(NODE_OPERATOR, current->valor);
        advance_token(parser);
        
        ASTNode *operator = parse_operator(parser);
        if (!operator) {
            free_ast(rel);
            return NULL;
        }
        add_child(rel, operator);
        
        ASTNode *next_rel = parse_rel(parser);
        if (next_rel) add_child(rel, next_rel);
        
        return rel;
    }
    
    return NULL; // ε production
}
