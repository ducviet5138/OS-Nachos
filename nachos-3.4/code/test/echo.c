#include "syscall.h"
#include "copyright.h"

#define MAX_LENGTH 255

int main()
{
    char string[MAX_LENGTH];

    Write("Input String: ", MAX_LENGTH, ConsoleOutput);
    Read(string, MAX_LENGTH, ConsoleInput);
    Write("Result      : ", MAX_LENGTH, ConsoleOutput);
    Write(string, MAX_LENGTH, ConsoleOutput);
    Write("\n", 1, ConsoleOutput);

    Halt();
}
