// compilas/main.c

#include <stdio.h>
#include <stdlib.h>
#include "../include/token.h"
#include "../include/parser.h"
#include "../include/ast.h"
#include "../include/reader.h"

// Declarações externas
char *readStr();
char *readFile(const char* filename);
Token** lexical_analyzer(const char *text);

int main() {
    char answer;
    char *conteudo = NULL;
    Token **tokens = NULL;
    Parser *parser = NULL;
    ASTNode *ast = NULL;

    printf("Deseja ler um arquivo ou digitar um texto?\n"
           "1 - Ler um arquivo\n"
           "2 - Digitar um texto\n");

    answer = getchar();
    getchar(); // Consume newline

    // Obter conteúdo
    switch(answer) {
        case '1': {
            char *nomeArqv = readStr();
            if (!nomeArqv) {
                fprintf(stderr, "Erro ao ler nome do arquivo\n");
                return 1;
            }
            
            conteudo = readFile(nomeArqv);
            if (!conteudo) {
                fprintf(stderr, "Erro ao ler arquivo %s\n", nomeArqv);
                free(nomeArqv);
                return 1;
            }
            
            printf("-----Conteudo-----\n%s\n------\n", conteudo);
            free(nomeArqv);
            break;
        }
        case '2': {
            conteudo = readStr();
            if (!conteudo) {
                fprintf(stderr, "Erro ao ler entrada\n");
                return 1;
            }
            
            printf("-----Resultado-----\nTexto digitado: %s------\n", conteudo);
            break;
        }
        default:
            printf("Opção inválida\n");
            return 1;
    }

    printf("OBTENÇÃO DO CONTEÚDO COM SUCESSO\n");

    // Análise léxica
    tokens = lexical_analyzer(conteudo);
    if (!tokens) {
        fprintf(stderr, "Erro na análise léxica\n");
        free(conteudo);
        return 1;
    }

    // Análise sintática
    parser = create_parser(tokens);
    if (!parser) {
        fprintf(stderr, "Erro ao criar parser\n");
        // Liberar tokens
        free(conteudo);
        return 1;
    }

    ast = parse_program(parser);
    
    // Verificar resultado da análise sintática
    if (ast) {
        printf("\n=== Árvore Sintática Abstrata ===\n");
        print_ast(ast, 0);
        printf("\nAnálise sintática concluída com sucesso!\n");
    } else {
        fprintf(stderr, "Erro na análise sintática\n");
    }
    

    // Limpeza de memória
    if (ast) free_ast(ast);
    if (parser) free_parser(parser);
    if (conteudo) free(conteudo);
    
    // Liberar tokens (você precisa implementar esta função)
    if (tokens) {
        for (int i = 0; tokens[i] != NULL; i++) {
            free(tokens[i]->nome);
            free(tokens[i]->valor);
            free(tokens[i]->tipo);
            free(tokens[i]);
        }
        free(tokens);
    }

    return 0;
}