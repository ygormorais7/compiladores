#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

char *readFile(const char* filename) {
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

char *readStr(){
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

char* readSubStr(char* str, int start, int end){
    int subLen = end - start + 1;
    
    char* subStr = malloc((subLen + 1) * sizeof(char));
    strncpy(subStr, str + start, subLen);
    subStr[subLen] = '\0';

    return subStr;
}

bool isDelimiter(char chr){
    return (   chr == ' ' || chr == '+' || chr == '-' || chr == '*' || chr == '/'
            || chr == '>' || chr == '<' || chr == '=' || chr == '(' || chr == ')'
            || chr == '\n'|| chr == '\t'
           );
}

bool isOperator(char chr){
    return (   chr == '+' || chr == '-' || chr == '*'
            || chr == '/' || chr == '>' || chr == '<'
            || chr == '=');
}

bool isInteger(char* str){
    if (str == NULL || *str == '\0') {
        return false;
    }
    int i = 0;
    while (isdigit(str[i])) {
        i++;
    }
    return str[i] == '\0';
}

bool isBooleanKeyword(const char* str) {
    return (strcmp(str, "falso") == 0 || strcmp(str, "verdadeiro") == 0);
}

bool isPrintKeyword(const char* str) {
    return strcmp(str, "imprime") == 0;
}

int isReserved(char* str) {
    if (isBooleanKeyword(str)) {
        return 0; // 0 = é reservado e é boolean
    } else if (isPrintKeyword(str)) {
        return 1; // 1 = é reservado e é imprime
    }
    return 2; // 2 = não é reservado
}

bool isValidIdentifier(char* str){
    return !isdigit(str[0]) && !isDelimiter(str[0]);
}


void teste(char *str){
    char ch;
    int left = 0, right = 0;

    while(str[right] != '\0'){
        right++;
    }

}