#include <xph/sys.h>

int main(int argc, char *argv[])
{
    int i;

    for (i = 1; i < argc; rmkfile(argv[i++], 0755)) {}

    return 0;
}
