#include <indexes.h>

void refresh_index(struct index* ind, int target) {
    if(target < ind->up) {
        ind->down -= ind->up - target;
        ind->up = target;
    } else if(target == ind->down) {
        ind->down++;
        ind->up++;
    }
}