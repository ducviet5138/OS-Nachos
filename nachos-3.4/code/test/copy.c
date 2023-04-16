#include "syscall.h"
#include "copyright.h"

#define MAX_LENGTH 255

int main()
{
    char source[MAX_LENGTH], dest[MAX_LENGTH], data[MAX_LENGTH];
    int s_fid = 0, d_fid = 0, length = 0;

    Write("Input Source's Filename: ", MAX_LENGTH, ConsoleOutput);
    Read(source, MAX_LENGTH, ConsoleInput);
    s_fid = Open(source, 1); // 1 == READONLY

    if (s_fid != -1)
    {
        Write("File ID: ", MAX_LENGTH, ConsoleOutput);
        WriteInt(s_fid);
        Write("\n", 1, ConsoleOutput);

        Write("Input Dest's Filename: ", MAX_LENGTH, ConsoleOutput);
        Read(dest, MAX_LENGTH, ConsoleInput);
        d_fid = Open(dest, 0); // 0 == READWRITE

        if (d_fid == -1) 
        {
            CreateFile(dest);
            d_fid = Open(dest, 0);
        }

        Write("File ID: ", MAX_LENGTH, ConsoleOutput);
        WriteInt(d_fid);
        Write("\n", 1, ConsoleOutput);

        
        do
        {
            length = Read(data, MAX_LENGTH, s_fid);
            Write(data, MAX_LENGTH, d_fid);
        } while (length == MAX_LENGTH);
    } else
    {
        Write("Source file does not exist", MAX_LENGTH, ConsoleOutput);
        Write("\n", 1, ConsoleOutput);
    }

    Halt();
}
