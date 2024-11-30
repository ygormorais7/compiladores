#include "../include/symbolTable.h"
#include <stdlib.h>
#include <string.h>

SymbolTable* create_symbol_table() {
    SymbolTable *table = (SymbolTable *)malloc(sizeof(SymbolTable));
    table->count = 0;
    table->capacity = 10;
    table->symbols = (Symbol **)malloc(table->capacity * sizeof(Symbol *));
    return table;
}

void free_symbol_table(SymbolTable *table) {
    for (int i = 0; i < table->count; i++) {
        free(table->symbols[i]->name);
        free(table->symbols[i]->type);
        free(table->symbols[i]);
    }
    free(table->symbols);
    free(table);
}

void add_symbol(SymbolTable *table, const char *name, const char *type, bool initialized) {
    if (table->count >= table->capacity) {
        table->capacity *= 2;
        table->symbols = (Symbol **)realloc(table->symbols, table->capacity * sizeof(Symbol *));
    }
    Symbol *symbol = (Symbol *)malloc(sizeof(Symbol));
    symbol->name = strdup(name);
    symbol->type = strdup(type);
    symbol->initialized = initialized;
    table->symbols[table->count++] = symbol;
}

Symbol* find_symbol(SymbolTable *table, const char *name) {
    for (int i = 0; i < table->count; i++) {
        if (strcmp(table->symbols[i]->name, name) == 0) {
            return table->symbols[i];
        }
    }
    return NULL;
}