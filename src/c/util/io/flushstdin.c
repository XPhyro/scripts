#include <unistd.h>

int main(int argc, char *argv[])
{
    unsigned char buf[2048];

    while (read(STDIN_FILENO, buf, sizeof(buf)))
        ;

    return 0;
}
