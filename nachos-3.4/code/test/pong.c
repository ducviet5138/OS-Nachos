#include "syscall.h"

#define MAX_LENGTH 255

int main()
{
    int i;
    for (i = 0; i < 1000; i++) Write("B", MAX_LENGTH, ConsoleOutput);
}
