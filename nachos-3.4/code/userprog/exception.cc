// exception.cc 
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.  
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "syscall.h"
#include "synchcons.h"
#include "cstring"
#include "math.h"

//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2. 
//
// And don't forget to increment the pc before returning. (Or else you'll
// loop making the same system call forever!
//
//	"which" is the kind of exception.  The list of possible exceptions 
//	are in machine.h.
//----------------------------------------------------------------------
#define MAX_LENGTH 255 

void IncProgramCounter()
{
    machine->WriteRegister(PrevPCReg, machine->ReadRegister(PCReg)); // registers[PrevPCReg] = registers[PCReg];
    machine->WriteRegister(PCReg, machine->ReadRegister(NextPCReg)); // registers[PCReg] = registers[NextPCReg];
    machine->WriteRegister(NextPCReg, machine->ReadRegister(NextPCReg) + 4); // registers[NextPCReg] = registers[NextPCReg + 4];
}

char* ConvertStringFromUserToSys(int UserMemAdd)
{
    char *string = new char[MAX_LENGTH + 1];

    for (int i = 0; i < MAX_LENGTH; i++)
    {
        int TempChar = 0;

        machine->ReadMem(UserMemAdd + i, 1, &TempChar);
        string[i] = (char)TempChar;

        if (TempChar == '\0') break;
    }

    return string;
}

int ConvertStringFromSysToUser(int UserAddMem, char *string)
{
    if (!string) return -1;
    
    int i = 0;
    int TempChar = 0;
    do
    {
        TempChar = (int) string[i];
        machine->WriteMem(UserAddMem + (i++), 1, TempChar);
    } while (i < MAX_LENGTH && TempChar != '\0');
    
    return i;
}

void ExceptionHandler(ExceptionType which)
{
    int type = machine->ReadRegister(2);
   
    switch(which)
    {
        case NoException:
            return;

        case PageFaultException:
            DEBUG('a', "[Error!] Page Fault Exception\n");
            printf("[Error!] Page Fault Exception\n");
            interrupt->Halt();
            break;

        case ReadOnlyException:
            DEBUG('a', "[Error!] Read Only Exception\n");
            printf("[Error!] Read Only Exception\n");
            interrupt->Halt();
            break;
        
        case BusErrorException:
            DEBUG('a', "[Error!] Bus Error Exception\n");
            printf("[Error!] Bus Error Exception\n");
            interrupt->Halt();
            break;

        case AddressErrorException:
            DEBUG('a', "[Error!] Address Error Exception\n");
            printf("[Error!] Address Error Exception\n");
            interrupt->Halt();
            break;

        case OverflowException:
            DEBUG('a', "[Error!] Overflow Exception\n");
            printf("[Error!] Overflow Exception\n");
            interrupt->Halt();
            break;

        case IllegalInstrException:
            DEBUG('a', "[Error!] Illegal Instr Exception\n");
            printf("[Error!] Illegal Instr Exception\n");
            interrupt->Halt();
            break;

        case NumExceptionTypes:
            DEBUG('a', "[Error!] Num Exception Types\n");
            printf("[Error!] Num Exception Types\n");
            interrupt->Halt();
            break;

        case SyscallException:
        {
            switch (type)
            {
                case SC_Halt:
                    DEBUG('a', "[Shutdown] Initiated by user program.\n");
                    printf("[Shutdown] Initiated by user program.\n");
                    interrupt->Halt();
                    break;

                case SC_CreateFile:
                {
                    DEBUG('a', "[Debug] Create a file\n");

                    int UserMemAdd = machine->ReadRegister(4);
                    char *filename = ConvertStringFromUserToSys(UserMemAdd);
                    
                    if (!filename || filename[0] == '\0' || !fileSystem->Create(filename, 0)) 
                    machine->WriteRegister(2, -1); else machine->WriteRegister(2, 0);
                    
                    delete []filename;
                    filename = NULL;
                    break;
                }

                case SC_Delete:
                {
                    DEBUG('a', "[Debug] Delete a file\n");

                    int UserAddMem = machine->ReadRegister(4);
                    char *filename = ConvertStringFromUserToSys(UserAddMem);

                    machine->WriteRegister(2,fileSystem->fl->Delete(filename));

                    delete []filename;
                    filename = NULL;
                    break;
                }

                case SC_Close:
                {
                    DEBUG('a', "[Debug] Close a file\n");

                    int fid = machine->ReadRegister(4);
                    machine->WriteRegister(2,fileSystem->fl->Close(fid));

                    break;
                }

                case SC_WriteInt:
                {
                    DEBUG('a', "[Debug] Print an integer to console\n");

                    int num = machine->ReadRegister(4);
                    bool NegativeNum = num < 0 ? 1 : 0;
                    if (num < 0) num *= -1;
                    int ArrLength = (int) log10(num) + 1;
                    char *res = new char [ArrLength + 1];

                    if (num == 0) gSynchConsole->Write("0", 1); else
                    {
                        res[ArrLength] = '\0';

                        for (int i = ArrLength - 1; i >= 0; i--)
                        {
                            res[i] = (num % 10) + 48;
                            num /= 10;
                        }
                    }
                    
                    if (NegativeNum) gSynchConsole->Write("-", 1);
                    gSynchConsole->Write(res, ArrLength);

                    delete []res;
                    res = NULL;
                    break;
                }

                case SC_Open:
                {
                    DEBUG('a', "[Debug] Open a file\n");

                    int UserMemAdd = machine->ReadRegister(4);
                    int type = machine->ReadRegister(5);
                    char *filename = ConvertStringFromUserToSys(UserMemAdd);

                    int ExitCode = fileSystem->fl->Open(filename, type);
                    machine->WriteRegister(2, ExitCode);

                    delete []filename;
                    filename = NULL;
                    break;
                }

                case SC_Write:
                {
                    DEBUG('a', "[Debug] Write into a file\n");

                    int UserMemAdd = machine->ReadRegister(4);
                    int count = machine->ReadRegister(5);
                    int fid = machine->ReadRegister(6);

                    if (fid == 0) machine->WriteRegister(2, -1); else // Cannot write to Console Input
                    if (fid == 1)
                    {
                        char *buffer = ConvertStringFromUserToSys(UserMemAdd);
                        if (!buffer) 
                        {
                            machine->WriteRegister(2, -1);
                            break;
                        }

                        gSynchConsole->Write(buffer, strlen(buffer));
                        machine->WriteRegister(2, strlen(buffer)); // Return a meaningful value instead of -2.

                        delete []buffer;
                        buffer = NULL;
                    } else
                    {
                        char *buffer = ConvertStringFromUserToSys(UserMemAdd);
                        int ExitCode = fileSystem->fl->Write(buffer, count, fid);
                        machine->WriteRegister(2, ExitCode);

                        delete []buffer;
                        buffer = NULL;
                    }            
                    break;
                }

                case SC_Read:
                {
                    DEBUG('a', "[Debug] Read from a file\n");

                    int UserMemAdd = machine->ReadRegister(4);
                    int count = machine->ReadRegister(5);
                    int fid = machine->ReadRegister(6);

                    if (fid == 1) machine->WriteRegister(2, -1); else // Cannot read from console output
                    if (fid == 0)
                    {
                        char *buffer = new char [MAX_LENGTH + 1];
                        for (int i = 0; i <= MAX_LENGTH; i++) buffer[i] = '\0';

                        gSynchConsole->Read(buffer, MAX_LENGTH);
                        ConvertStringFromSysToUser(UserMemAdd, buffer);
                        machine->WriteRegister(2, strlen(buffer)); // Return a meaningful value instead of -2
                        delete []buffer;
                        buffer = NULL;
                    } else
                    {
                        char *buffer = new char [MAX_LENGTH + 1];
                        for (int i = 0; i <= MAX_LENGTH; i++) buffer[i] = '\0';
                        int ExitCode = fileSystem->fl->Read(buffer, count, fid);
                        
                        ConvertStringFromSysToUser(UserMemAdd, buffer);
                        machine->WriteRegister(2, ExitCode);

                        delete []buffer;
                        buffer = NULL;
                    }
                
                    
                    break;
                }

                case SC_Seek:
                {
                    DEBUG('a', "[Debug] Seek to a pos of file\n");

                    int pos = machine->ReadRegister(4);
                    int fid = machine->ReadRegister(5);

                    if (fid == 1 || fid == 0) machine->WriteRegister(2, -1); else // Cannot Seek the console
                    {
                        int ExitCode = fileSystem->fl->Seek(pos, fid);
                        machine->WriteRegister(2, ExitCode);
                    }
                
                    break;
                }

                case SC_Exec:
                {
                    DEBUG('a', "[Debug] Execute a thread\n");

                    int UserMemAdd = machine->ReadRegister(4);
                    char* name = ConvertStringFromUserToSys(UserMemAdd);

                    int sid = nPageTable->Exec(name);
                    machine->WriteRegister(2, sid);

                    delete name;
                    name = NULL;
                    break;
                }

                case SC_Exit:
                {
                    int ExitCode = machine->ReadRegister(4);
                    if (ExitCode != 0) break;

                    currentThread->FreeSpace();
                    currentThread->Finish();

                    break;
                }

                case SC_Join:
                case SC_Fork:
                case SC_Yield:
                    DEBUG('a', "[Error!] Unimplemented Syscall.\n");
                    printf("[Error!] Unimplemented Syscall.\n");
                    interrupt->Halt();
                    break;

                        
                default:
                    printf("Unexpected user mode exception %d %d\n", which, type);
                    ASSERT(FALSE);
                    break;
            }
        }

        IncProgramCounter();
    }
}
