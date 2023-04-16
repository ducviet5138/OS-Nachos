#include "syscall.h"
#include "copyright.h"

#define MAX_LENGTH 255

int main()
{
    char filename[MAX_LENGTH], data[MAX_LENGTH];
    int fid = 0, length = 0;

    Write("Input Filename: ", MAX_LENGTH, ConsoleOutput);
    Read(filename, MAX_LENGTH, ConsoleInput);

    fid = Open(filename, 1); // 1 == READONLY

    if (fid != -1)
    {
        Write("File ID: ", MAX_LENGTH, ConsoleOutput);
        WriteInt(fid);
        Write("\n", 1, ConsoleOutput);

        Write("Data: \n", MAX_LENGTH, ConsoleOutput);
        do
        {
            length = Read(data, MAX_LENGTH, fid);
            Write(data, MAX_LENGTH, ConsoleOutput);
        } while (length == MAX_LENGTH);
        Write("\n", 1, ConsoleOutput);
    } else Write("File does not exist.\n", MAX_LENGTH, ConsoleOutput);
    
    Halt();
}
