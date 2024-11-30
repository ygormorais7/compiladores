#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "estruturas.h"

// Funções importadas
char* readSubStr(char* str, int start, int end);
bool isDelimiter(char chr);
bool isOperator(char chr);
bool isInteger(char* str);
int isReserved(char* str);
bool isValidIdentifier(char* str);
void initStringArray(struct StringArray* arr);
void addString(struct StringArray* arr, const char* format, const char* value);
void freeStrings(struct StringArray* arr);

struct StringArray lexer(char* programa) {
    struct StringArray arr;
    initStringArray(&arr);

    int left = 0, right = 0;
    int len = strlen(programa);
    char stringCopia[3];
    char tempStr[2];  // String temporária para armazenar um caractere

    while (right <= len && left <= right) {
        if (!isDelimiter(programa[right])) {
            right++;
        }

        if (isDelimiter(programa[right]) && left == right) {
            if (isOperator(programa[right])) {
                if (programa[right] == '=' && programa[right + 1] == '=') {
                    printf("[RELOP, Value: ==]\n");
                    strcpy(stringCopia, "==");
                    addString(&arr, "[RELOP, Value: %s]", stringCopia);
                    right++;
                } else if (programa[right] == '!' && programa[right + 1] == '=') {
                    printf("[RELOP, Value: !=]\n");
                    strcpy(stringCopia, "!=");
                    addString(&arr, "[RELOP, Value: %s]", stringCopia);
                    right++;
                }
            } else {
                tempStr[0] = programa[right];
                tempStr[1] = '\0';
                if (programa[right] == '(') {
                    printf("[LPAREN, Value: (]\n");
                    addString(&arr, "[LPAREN, Value: %s]", tempStr);
                } else if (programa[right] == ')') {
                    printf("[RPAREN, Value: )]\n");
                    addString(&arr, "[RPAREN, Value: %s]", tempStr);
                } else if (programa[right] == '\n') {
                    printf("[DELIMITER, Value: \\n]\n");
                    addString(&arr, "[DELIMITER, Value: %s]", "\\n");
                } else {
                    printf("[OP, Value: %c]\n", programa[right]);
                    addString(&arr, "[OP, Value: %s]", tempStr);
                }
            }
            right++;
            left = right;
        } else if (isDelimiter(programa[right]) && left != right || (right == len && left != right)) {
            char* subStr = readSubStr(programa, left, right - 1);

            // Reservados
            if (isReserved(subStr) == 0) {
                printf("[BOOLEAN, Value: %s]\n", subStr);
                addString(&arr, "[BOOLEAN, Value: %s]", subStr);
            } else if (isReserved(subStr) == 1) {
                printf("[RESERVED, Value: %s]\n", subStr);
                addString(&arr, "[RESERVED, Value: %s]", subStr);
            }
            // Inteiros
            else if (isInteger(subStr)) {
                printf("[INT, Value: %s]\n", subStr);
                addString(&arr, "[INT, Value: %s]", subStr);
            }
            // Identificadores
            else if (isValidIdentifier(subStr) && !isDelimiter(programa[right - 1])) {
                printf("[ID, Value: %s]\n", subStr);
                addString(&arr, "[ID, Value: %s]", subStr);
            }
            // Invalidos
            else if (!isValidIdentifier(subStr) && !isDelimiter(programa[right - 1])) {
                printf("[Unidentified, Value: %s]\n", subStr);
            }
            left = right;
        }
    }

    return arr;
}