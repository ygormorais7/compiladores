#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

// Definindo a estrutura do token
typedef struct {
    char* tipo;
    char* valor;
} Token;

// Definindo um array de tokens como exemplo
Token tokens[] = {
    {"ID", "resultado"}, {"OP", "="}, {"INT", "2"}, {"DELIMITER", "\n"},
    {"ID", "resultado2"}, {"OP", "="}, {"INT", "3"}, {"DELIMITER", "\n"},
    {"ID", "r3"}, {"OP", "="}, {"ID", "resultado2"}, {"OP", "*"}, {"ID", "resultado"}, {"DELIMITER", "\n"},
    {"RESERVED", "imprime"}, {"LPAREN", "("}, {"ID", "r3"}, {"RPAREN", ")"}, {"DELIMITER", "\n"}
};
int currentTokenIndex = 0;
Token* current_token;

// Estrutura para armazenar variáveis e seus valores
typedef struct {
    char* nome;
    int valor;
} Variavel;

Variavel variaveis[100];  // Supondo um máximo de 100 variáveis
int numVariaveis = 0;

// Função para pegar o próximo token
void nextToken() {
    if (currentTokenIndex < sizeof(tokens) / sizeof(tokens[0])) {
        current_token = &tokens[currentTokenIndex++];
    } else {
        current_token = NULL;  // Indica o fim dos tokens
    }
}

// Função para consumir um token específico
bool consume(char* tipo, char* valor) {
    if (current_token != NULL && strcmp(current_token->tipo, tipo) == 0 && strcmp(current_token->valor, valor) == 0) {
        nextToken();
        return true;
    }
    return false;
}

// Função para verificar se é identificador
bool identificador(char** nome) {
    if (current_token != NULL && strcmp(current_token->tipo, "ID") == 0) {
        *nome = current_token->valor;
        nextToken();
        return true;
    }
    return false;
}

// Função para verificar se é número
bool numero(int* valor) {
    if (current_token != NULL && strcmp(current_token->tipo, "INT") == 0) {
        *valor = atoi(current_token->valor);
        nextToken();
        return true;
    }
    return false;
}

// Prototipação das funções
bool programa();
bool programaLinha();
bool comando();
bool expressao(int* valor);
bool expressaoLinha(int* valor);
bool termo(int* valor);
bool termoLinha(int* valor);
bool fator(int* valor);
bool logica(int* valor);
bool logicaLinha(int* valor);

// Função para buscar o valor de uma variável
int buscarVariavel(char* nome) {
    for (int i = 0; i < numVariaveis; i++) {
        if (strcmp(variaveis[i].nome, nome) == 0) {
            return variaveis[i].valor;
        }
    }
    return 0;  // Retorna 0 se a variável não for encontrada
}

// Função para definir o valor de uma variável
void definirVariavel(char* nome, int valor) {
    for (int i = 0; i < numVariaveis; i++) {
        if (strcmp(variaveis[i].nome, nome) == 0) {
            variaveis[i].valor = valor;
            return;
        }
    }
    variaveis[numVariaveis].nome = strdup(nome);
    variaveis[numVariaveis].valor = valor;
    numVariaveis++;
}

// Implementação da gramática
bool programa() {
    if (comando()) {
        return programaLinha();
    }
    return true;  // ε (opcional)
}

bool programaLinha() {
    if (consume("DELIMITER", "\n")) {
        if (comando()) {
            return programaLinha();
        }
    }
    return true;  // ε (opcional)
}

bool comando() {
    char* varNome;
    int valor;
    // Verifica se é identificador
    if (identificador(&varNome)) {
        // Se for, verifica se o próximo token é atribuição
        if (consume("OP", "=")) {

            if (expressao(&valor)) {
                definirVariavel(varNome, valor);
                return true;
            }
        }
    } else if (consume("RESERVED", "imprime")) {
        if (consume("LPAREN", "(")) {
            if (expressao(&valor)) {
                printf("%d\n", valor);
                return consume("RPAREN", ")");
            }
        }
    }
    return false;
}

bool expressao(int* valor) {
    if (termo(valor)) {
        return expressaoLinha(valor);
    }
    return false;
}

bool expressaoLinha(int* valor) {
    int termoValor;
    if (consume("OP", "+")) {
        if (termo(&termoValor)) {
            *valor += termoValor;
            return expressaoLinha(valor);
        }
    } else if (consume("OP", "-")) {
        if (termo(&termoValor)) {
            *valor -= termoValor;
            return expressaoLinha(valor);
        }
    }
    return true;  // ε (opcional)
}

bool termo(int* valor) {
    if (fator(valor)) {
        return termoLinha(valor);
    }
    return false;
}

bool termoLinha(int* valor) {
    int fatorValor;
    if (consume("OP", "*")) {
        if (fator(&fatorValor)) {
            *valor *= fatorValor;
            return termoLinha(valor);
        }
    } else if (consume("OP", "/")) {
        if (fator(&fatorValor)) {
            *valor /= fatorValor;
            return termoLinha(valor);
        }
    }
    return true;  // ε (opcional)
}

bool fator(int* valor) {
    char* varNome;
    if (consume("LPAREN", "(")) {
        if (expressao(valor)) {
            return consume("RPAREN", ")");
        }
    } else if (identificador(&varNome)) {
        *valor = buscarVariavel(varNome);
        return true;
    } else if (numero(valor)) {
        return true;
    } else if (logica(valor)) {
        return true;
    }
    return false;
}

bool logica(int* valor) {
    if (fator(valor)) {
        return logicaLinha(valor);
    }
    return false;
}

bool logicaLinha(int* valor) {
    int fatorValor;
    if (consume("RELOP", "&&")) {
        if (fator(&fatorValor)) {
            *valor = *valor && fatorValor;
            return logicaLinha(valor);
        }
    } else if (consume("RELOP", "||")) {
        if (fator(&fatorValor)) {
            *valor = *valor || fatorValor;
            return logicaLinha(valor);
        }
    } else if (consume("RELOP", "!")) {
        if (fator(&fatorValor)) {
            *valor = !fatorValor;
            return logicaLinha(valor);
        }
    }
    return true;  // ε (opcional)
}

// Função principal para teste
int main() {
    nextToken();  // Iniciar com o primeiro token
    if (programa()) {
        printf("Parsing successful!\n");
    } else {
        printf("Parsing failed!\n");
    }
    return 0;
}