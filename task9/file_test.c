#include <stdio.h>
#define LINE_SIZE 16
int main()
{
    char line[LINE_SIZE] = "String from file";
    FILE* file = fopen("./output", "w+");
    fwrite(line, sizeof(char), LINE_SIZE, file);

    char new_line[LINE_SIZE];
    fseek(file, 0, SEEK_SET);
    fread(new_line, sizeof(char), LINE_SIZE, file);
    printf("%s", new_line);
    fclose(file);
    return 0;
}