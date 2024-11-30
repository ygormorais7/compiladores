#include <stdio.h>
#include <stdlib.h>
#include "estruturas.h"

// Funções importadas
char *readFile(const char *filename);
char *readStr();
struct StringArray lexer(char* programa);

int main() {
    char resposta;

    printf("Deseja ler um arquivo ou digitar um texto?\n"
            "1 - Ler um arquivo\n"
            "2 - Digitar um texto\n");

    resposta = getchar();
    getchar();

    char *conteudo = NULL;
    struct StringArray tokens;

    switch (resposta) {
        case '1': {
            char *nomeArqv = readStr();
            conteudo = readFile(nomeArqv);
            printf("-----Resultado-----\n"
                   "Conteúdo do arquivo:\n%s\n------\n", conteudo);

            tokens = lexer(conteudo);

            printf("Tokens:\n");
            for (int i = 0; i < tokens.count; i++) {
                printf("%s\n", tokens.strings[i]);
            }

            break;
        }
        case '2': {
            conteudo = readStr();
            printf("-----Resultado-----\n"
                   "Texto digitado: %s\n", conteudo);

            tokens = lexer(conteudo);

            printf("Tokens:\n");
            for (int i = 0; i < tokens.count; i++) {
                printf("%s\n", tokens.strings[i]);
            }

            break;
        }
        default:
            printf("Opção inválida\n");
            break;
    }

    // Liberar a memória alocada para os tokens
    freeStrings(&tokens);

    return 0;
}