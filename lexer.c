#include <stdio.h>
#include <stdbool.h>

bool isDelimiter(char chr){
    return (chr == ' '
            || chr == '+' || chr == '-' || chr == '*' || chr == '/'
            || chr == '>' || chr == '<' || chr == '='
            || chr == ')' || chr == '\n' || chr == '\t'
            );
}

int analiseLexica(char* programa){
    int pont1 = 0, pont2 = 0;
    int len = strlen(programa);

    while(pont2 <= len && pont2 <= pont1){
        if (!isDelimiter(programa[pont2])){
            pont2++;
        }
    }
}