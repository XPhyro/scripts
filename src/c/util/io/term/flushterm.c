#include <termios.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    if (tcflush(STDIN_FILENO, TCIOFLUSH))
        return 1;
    return 0;
}
