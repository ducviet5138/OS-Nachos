#ifndef NEWPAGETABLE_H
#define NEWPAGETABLE_H

#include "bitmap.h"
#include "thread.h"
#include "string.h"

#define MAX_PROCESS 10

typedef int SpaceId;
extern void MyStartProcess(int sid);

class NewPageTable
{
private:  
    BitMap *Process;
    char **ProcessName;
public:
    NewPageTable()
    {
        Process = new BitMap(MAX_PROCESS);
        ProcessName = new char* [MAX_PROCESS];
        Process->Mark(0);
    }

    ~NewPageTable()
    {
        delete Process;
        
        for (int i = 0; i < MAX_PROCESS; i++) 
        {
            delete ProcessName[i];
            ProcessName[i] = NULL;
        }

        delete ProcessName;
        ProcessName = NULL;
    }

    char *GetName(int sid)
    {
        if (sid < 0 || sid > MAX_PROCESS - 1 || !Process->Test(sid)) return "";
        return ProcessName[sid];
    }

    SpaceId Exec(char* name)
    {
        if (!name || name[0] == '\0') return -1;

        int sid = Process->Find();
        if (sid == -1) return -1;

        ProcessName[sid] = new char [strlen(name) + 1];
        strcpy(ProcessName[sid], name);

        Thread *NewThread = new Thread(name);
        NewThread->Fork(MyStartProcess, sid);
        return sid;
    }
};

#endif