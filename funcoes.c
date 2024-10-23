#include <stdio.h>
#include <stdlib.h>

char *lerArquivo(const char *filename) {
    FILE *file = fopen(filename, "r");
    char *content = NULL;
    long file_size;

    if (file != NULL) {
        fseek(file, 0, SEEK_END);
        file_size = ftell(file);
        rewind(file);

        content = malloc((file_size + 1) * sizeof(char));
        if (content != NULL) {
            fread(content, sizeof(char), file_size, file);
            content[file_size] = '\0';
        }

        fclose(file);
    } else {
        fprintf(stderr, "Unable to open file!\n");
        return NULL;
    }

    return content;
}

char *lerNome(){
    int tamanho = 1;
    char c;
    int i = 0;
    char *nome = malloc(tamanho * sizeof(char));

    printf("Digite o nome:\n");

    while ((c = getchar()) != '\n') {
        nome[i] = c;
        i++;
    
        // Realoca a memória se necessário
        if (i >= tamanho) {
            tamanho++;
            nome = realloc(nome, tamanho * sizeof(char));
            if (nome == NULL) {
                printf("Erro na realocação de memória.\n");
                return "erro";
            }
        }
    }
    return nome;
}