#include <stdio.h>
#include <stdlib.h>

// Funções importadas
char *readFile(const char *filename);
char *readStr();

int lexer(char* programa);

int main(){

    char resposta;

    printf("Deseja ler um arquivo ou digitar um texto?\n"
            "1 - Ler um arquivo\n"
            "2 - Digitar um texto\n");

    resposta = getchar();
    getchar();

    switch(resposta){
        case '1':
            char *nomeArqv = readStr();
            char *conteudo = readFile(nomeArqv);
            printf("-----Resultado-----\n"
            "Conteúdo do arquivo:\n%s\n------\n", conteudo);

            lexer(conteudo);

            break;

        case '2':
            char *texto = readStr();
            printf("-----Resultado-----\n"
                "Texto digitado: %s\n", texto);
            break;

        default:
            printf("Opção inválida\n");
            break;
    }
}