#include <stdio.h>

int main(int argc, char *argv[])
{
    int c;

    while ((c = getchar()) != '\n' && c != EOF) {}

    return 0;
}
