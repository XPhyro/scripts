#include <limits.h>
#include <stdint.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    const uint64_t data[] = { UINT64_MAX };
    for (;; write(1, data, sizeof(data[0]))) {}
}
