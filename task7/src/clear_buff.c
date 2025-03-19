#include <stdio.h>
#include <clear_buff.h>

void clear_buff() {
    while (getchar() != '\n');
}