#include <termios.h>

int main(int argc, char *argv[])
{
    if (tcflush(0, TCIOFLUSH))
        return 1;
    return 0;
}
