#include <stdio.h>
#include <stdlib.h>

int main(){
    char resposta;
    char *lerArquivo(const char *filename);
    char *lerNome();

    printf("Deseja ler um arquivo ou digitar um texto?\n"
            "1 - Ler um arquivo\n"
            "2 - Digitar um texto\n");

    resposta = getchar();
    getchar();

    switch(resposta){
        case '1':
            char *nomeArqv = lerNome();
            char *conteudo = lerArquivo(nomeArqv);
            printf("-----Resultado-----\n"
            "Conteúdo do arquivo: %s\n", conteudo);
            break;

        case '2':
            char *texto = lerNome();
            printf("-----Resultado-----\n"
                "Texto digitado: %s\n", texto);
            break;

        default:
            printf("Opção inválida\n");
            break;
    }
}