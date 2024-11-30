#ifndef AST_H
#define AST_H

#include <stdbool.h>
#include "token.h"

typedef enum {
    NODE_PROGRAM,
    NODE_DECL,
    NODE_STMT,
    NODE_PRINT,
    NODE_ASSIGN,
    NODE_EXPR,
    NODE_TERM,
    NODE_FACTOR,
    NODE_ID,
    NODE_NUM,
    NODE_BOOL,
    NODE_OPERATOR
} NodeType;

typedef struct ASTNode {
    NodeType type;
    char *value;
    char *dataType;
    struct ASTNode **children;
    int num_children;
} ASTNode;

ASTNode* create_ast_node(NodeType type, const char *value);
void add_child(ASTNode *parent, ASTNode *child);
void free_ast(ASTNode *node);
void print_ast(ASTNode *node, int level);
const char* node_type_to_string(NodeType type);

#endif
