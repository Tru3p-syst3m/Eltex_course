#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* find_subbstr(char* str, char* sstr, int s_size, int ss_size){
    for(int i = 0; i <= s_size - ss_size; i++){
        if(*str == *sstr){
            char* border = str + ss_size;
            char buff = *border;
            *border = '\0';
            if(!strcmp(str, sstr)){
                return str;
            }
            *border = buff;
        }
        str++;
    }
    return NULL;
}

int main(){
    int str_size = 10, sstr_size = 10, i = 0;
    char* str = malloc(str_size);
    char* sstr = malloc(sstr_size);
    printf("Type the text: ");
    while(1){
        char c;
        scanf("%c", &c);
        if(c == '\n'){
            str_size = i;
            break;
        }
        if(i+1 ==  str_size){
            str = (char*)realloc(str, str_size * 2);
        }
        str[i] = c;
        str[i+1] = '\0';
        i++;
    }
    
    i = 0;

    printf("Type the substring: ");
    while(1){
        char c;
        scanf("%c", &c);
        if(c == '\n'){
            sstr_size = i;
            break;
        }
        if(i+1 ==  sstr_size){
            sstr = (char*)realloc(sstr, sstr_size * 2);
        }
        sstr[i] = c;
        sstr[i+1] = '\0';
        i++;
    }
    
    char* substr = find_subbstr(str, sstr, str_size, sstr_size);
    substr++;//for removing the warning
    free(str);
    free(sstr);
    return 0;
}