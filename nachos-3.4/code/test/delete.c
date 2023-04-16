#include "syscall.h"
#include "copyright.h"

#define MAX_LENGTH 255

int main()
{
    char filename[MAX_LENGTH], temp[MAX_LENGTH];
    int fid = 0, ExitCode = 0;

    Write("Input Filename: ", MAX_LENGTH, ConsoleOutput);
    Read(filename, MAX_LENGTH, ConsoleInput);

    fid = Open(filename, 1); // 1 == READONLY
    
    if (fid == -1)
    {
        Write("File does not exist. Creating....\n", MAX_LENGTH, ConsoleOutput);
        CreateFile(filename);
        fid = Open(filename, 1); // 1 == READONLY
    }

    Write("File is opened successfully with file id: ", MAX_LENGTH, ConsoleOutput);
    WriteInt(fid);
    Write("\nPress [Enter] to delete.", MAX_LENGTH, ConsoleOutput);
    Read(temp, MAX_LENGTH, ConsoleInput);
    ExitCode = Delete(filename);
    
    if (ExitCode == -1) 
    {
        // If this part is printed => Cannot delete the opened file
        Write("File is opened. Press [Enter] to close.", MAX_LENGTH, ConsoleOutput); 
        Read(temp, MAX_LENGTH, ConsoleInput);
        Close(fid);
    }

    Write("Press [Enter] to delete.", MAX_LENGTH, ConsoleOutput);
    Read(temp, MAX_LENGTH, ConsoleInput);
    ExitCode = Delete(filename);
    if (ExitCode == 0) Write("File is deleted successfully.\n", MAX_LENGTH, ConsoleOutput); else 
    Write("Cannot delete.\n", MAX_LENGTH, ConsoleOutput);


    Halt();
}
