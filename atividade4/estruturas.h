#ifndef ESTRUTURAS_H
#define ESTRUTURAS_H

// Estrutura para armazenar até 100 strings
#define MAX_STRING 100

struct StringArray {
    char* strings[MAX_STRING]; // Array de ponteiros para strings
    int count;                // Contador de quantas strings foram adicionadas
};

#endif // ESTRUTURAS_H