#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "../include/ast.h"

bool is_terminal(NodeType type, const char *value) {
    // Identificadores e números são sempre terminais
    if (type == NODE_ID || type == NODE_NUM || type == NODE_BOOL) {
        return true;
    }
    // Operadores são terminais quando têm um valor específico
    if (type == NODE_OPERATOR && value && strcmp(value, "operator") != 0) {
        return true;
    }
    // Símbolos especiais também são terminais
    if (value && (strcmp(value, "=") == 0 || 
                 strcmp(value, ";") == 0 ||
                 strcmp(value, "(") == 0 ||
                 strcmp(value, ")") == 0)) {
        return true;
    }
    return false;
}

ASTNode* create_ast_node(NodeType type, const char *value) {
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node) return NULL;
    
    node->type = type;
    node->value = value ? strdup(value) : NULL;
    node->dataType = NULL;
    node->children = malloc(sizeof(ASTNode*) * 10);
    if (!node->children) {
        free(node);
        return NULL;
    }
    
    for (int i = 0; i < 10; i++) {
        node->children[i] = NULL;
    }
    node->num_children = 0;
    
    return node;
}

void add_child(ASTNode *parent, ASTNode *child) {
    if (!parent || !child || !parent->children) return;
    if (parent->num_children >= 10) return;
    
    parent->children[parent->num_children++] = child;
}

void free_ast(ASTNode *node) {
    if (!node) return;
    
    for (int i = 0; i < node->num_children; i++) {
        free_ast(node->children[i]);
    }
    
    free(node->value);
    free(node->dataType);
    free(node->children);
    free(node);
}

const char* node_type_to_string(NodeType type) {
    switch (type) {
        case NODE_PROGRAM: return "PROGRAM";
        case NODE_DECL: return "DECL";
        case NODE_STMT: return "STMT";
        case NODE_PRINT: return "PRINT";
        case NODE_ASSIGN: return "ASSIGN";
        case NODE_EXPR: return "EXPR";
        case NODE_TERM: return "TERM";
        case NODE_FACTOR: return "FACTOR";
        case NODE_ID: return "ID";
        case NODE_NUM: return "NUM";
        case NODE_BOOL: return "BOOL";
        case NODE_OPERATOR: return "OPERATOR";
        default: return "UNKNOWN";
    }
}

void print_branch(int level, bool last, bool* is_last) {
    if (!is_last) return;
    
    for (int i = 0; i < level - 1; i++) {
        printf(is_last[i] ? "    " : "│   ");
    }
    if (level > 0) {
        printf(last ? "└── " : "├── ");
    }
}

void print_ast_improved(ASTNode *node, int level, bool last, bool* is_last) {
    if (!node || !is_last) return;

    print_branch(level, last, is_last);

    // Verifica se é um terminal para dar destaque especial
    if (is_terminal(node->type, node->value)) {
        printf("'%s'", node->value ? node->value : "");
    } else {
        printf("%s", node_type_to_string(node->type));
        if (node->value && !is_terminal(node->type, node->value)) {
            printf(" (%s)", node->value);
        }
    }
    printf("\n");

    if (node->children && node->num_children > 0) {
        if (level >= 0) {
            is_last[level] = last;
        }

        for (int i = 0; i < node->num_children; i++) {
            if (node->children[i]) {
                print_ast_improved(node->children[i], level + 1, 
                                 i == node->num_children - 1, is_last);
            }
        }
    }
}


void print_ast(ASTNode *node, int level) {
    if (!node) {
        printf("\nÁrvore vazia!\n");
        return;
    }

    printf("\n=== Árvore Sintática Abstrata ===\n\n");
    bool* is_last = calloc(100, sizeof(bool));
    if (!is_last) {
        printf("Erro de alocação de memória\n");
        return;
    }

    print_ast_improved(node, 0, true, is_last);
    free(is_last);
    printf("\n");
}
