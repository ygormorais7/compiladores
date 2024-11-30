#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <stdbool.h>

typedef struct {
    char *name;         // Nome do identificador
    char *type;         // Tipo do identificador
    bool initialized;   // Se foi inicializado
} Symbol;

typedef struct {
    Symbol **symbols;
    int count;
    int capacity;
} SymbolTable;

SymbolTable* create_symbol_table();
void free_symbol_table(SymbolTable *table);
void add_symbol(SymbolTable *table, const char *name, const char *type, bool initialized);
Symbol* find_symbol(SymbolTable *table, const char *name);

#endif