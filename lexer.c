#include <stdio.h>
#include <stdbool.h>
#include <string.h>

char* readSubStr(char* str, int start, int end);
bool isDelimiter(char chr);
bool isOperator(char chr);
bool isInteger(char* str);
int isReserved(char* str);
bool isValidIdentifier(char* str);

int lexer(char* programa){
    int left = 0, right = 0;
    int len = strlen(programa);

    while (right <= len && left <= right){
        if (!isDelimiter(programa[right])){
            right++;
        }

        if (isDelimiter(programa[right]) && left == right) {
            if (isOperator(programa[right])){
                printf("Token: (Operator, Value: %c)\n",programa[right]);
            }
            right++;
            left = right;
        }
        else if (isDelimiter(programa[right]) && left != right || (right == len && left != right)) {
            char* subStr = readSubStr(programa, left, right - 1);

            // Reservados
            if (isReserved(subStr) == 0){
                printf("Token: (Boolean, Value: %s)\n",subStr);
            }else if (isReserved(subStr) == 1){
                printf("Token: (Reserved, Value: %s)\n",subStr);
            }
            // Inteiros
            else if (isInteger(subStr)){
                printf("Token: (Integer, Value: %s)\n",subStr);
            }
            // Identificadores
            else if (isValidIdentifier(subStr) && !isDelimiter(programa[right - 1])){
                printf("Token: (Identifier, Value: %s)\n",subStr);
            }
            // Invalidos
            else if (!isValidIdentifier(subStr) && !isDelimiter(programa[right - 1])){
                printf("Token: (Unidentified, Value: %s)\n",subStr);
            }
            left = right;
        }
    }
}