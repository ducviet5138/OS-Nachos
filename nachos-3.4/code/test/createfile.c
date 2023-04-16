#include "syscall.h"
#include "copyright.h"

#define MAX_LENGTH 255

int main()
{
    char name[MAX_LENGTH];

    Write("Input Filename: ", MAX_LENGTH, ConsoleOutput);
    Read(name, MAX_LENGTH, ConsoleInput);

    if (CreateFile(name) == 0)
    {
        Write("Create file successfully.\n", MAX_LENGTH, ConsoleOutput);
    } else Write("Fail to create file.\n", MAX_LENGTH, ConsoleOutput);

    Halt();
}
