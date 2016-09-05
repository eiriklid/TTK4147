#include <stdlib.h>
#include <stdio.h>

/* 
Programmet tar opp alt minet på PCen. Dette skjer fordi vi aldri free'er minnet vi malloc'et
Minnebruken bare øker og øker i top
   */

void allocate(int value) {
    int *ptr = NULL;
    if ((ptr = (int*) malloc(100000 * sizeof(int))) == NULL) {
        perror("Error");
        return;
    }

    *ptr = value;
    printf("Memory allocated\n");
} 

int main() {
    while (1) {
        allocate(1);
    }
}
