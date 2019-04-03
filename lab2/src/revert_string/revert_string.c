#include "revert_string.h"

void RevertString(char *str)
{
    int count = 0;
    while (str[count] != '\0')
    count++;
    char temp;
    for (int i = 2; i < (count)/2; i++) {
        temp = str[i];
        str[i] = str[count-i-1];
        str[count-i-1]=temp;
    }

}

