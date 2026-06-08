#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: cat foo.gif | %s <microseconds> | teminalgif\n", argv[0]);
        return 1;
    }

    useconds_t delay = atoi(argv[1]);
    int c;

    while ((c = getchar()) != EOF) {
        usleep(delay);
        putchar(c);
    }

    return 0;
}

