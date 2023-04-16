#ifndef FILELIST_H
#define FILELIST_H
#include "bitmap.h"
#include "FileType.h"
#include "cstring"


typedef int OpenFileID;

class FileList
{
private:
    BitMap* bm;
    NewOpenFile **of; 
    char** filename;
public:
    FileList()
    {
        bm = new BitMap(MAX_FILE);
        of = new NewOpenFile*[MAX_FILE];
        filename = new char*[MAX_FILE];

        for (int i = 0; i < MAX_FILE; i++) of[i] = NULL;
        bm->Mark(0);
        bm->Mark(1);
    };

    ~FileList()
    {
        delete bm;
        bm = NULL;

        for (int i = 0; i < MAX_FILE; i++) 
        {
            delete of[i];
            of[i] = NULL;

            delete filename[i];
            filename[i] = NULL;
        }

        delete []of;
        of = NULL;

        delete []filename;
        filename = NULL;
    }

    int Open(char *name, int type)
    {
        if (type != READWRITE && type != READONLY) return -1;
        if (!name || name[0] == '\0' || isExistInList(name)) return -1;
        
        int index = bm->Find();
        if (index == -1) return -1;
        
        // Copy from "filesys.h"
        int fileDescriptor = OpenForReadWrite(name, 0);
        if (fileDescriptor == -1) 
        {
            bm->Clear(index);
            return -1;
        }

        of[index] = (type == READONLY) ? (NewOpenFile*) new ReadOnly(fileDescriptor) : (NewOpenFile*) new ReadWrite(fileDescriptor);
        filename[index] = CreateFileName(name);
        
        return index;
    }

    int Close(OpenFileID fid)
    {
        if (fid < 2 || fid > MAX_FILE - 1 || !bm->Test(fid)) return -1;

        delete of[fid];
        of[fid] = NULL;

        delete filename[fid];
        filename[fid] = NULL;

        bm->Clear(fid);

        return 0;
    }

    int Read(char* buffer, int count, OpenFileID fid)
    {
        if (fid < 2 || fid > MAX_FILE - 1 || !bm->Test(fid)) return -1;

        int pos = of[fid]->Tellg();
        int FileSize = of[fid]->Length();
        int RealSize = of[fid]->Read(buffer, count);  

        if (RealSize == -1) return -1; 
        return (pos + RealSize < FileSize - 1) ? RealSize : -2; // FileSize - 1 == Ignore the "end of file" character
    }

    int Write(char* buffer, int count, OpenFileID fid)
    {
        if (fid < 2 || fid > MAX_FILE - 1 || !bm->Test(fid)) return -1;

        int pos = of[fid]->Tellg();
        int FileSize = of[fid]->Length();
        int RealSize = of[fid]->Write(buffer, count);

        if (RealSize == -1) return -1;
        return (pos + RealSize < FileSize - 1) ? RealSize : -2; // FileSize - 1 == Ignore the "end of file" character
    }

    int Seek(int pos, OpenFileID fid)
    {
        if (pos < -1 || fid < 2 || fid > MAX_FILE - 1 || !bm->Test(fid) || pos > of[fid]->Length()) return -1;
        
        if (pos == -1) pos = of[fid]->Length();
        of[fid]->Seek(pos);  
        return pos;
    }

    int Delete(char* name)
    {
        if (!name || name[0] == '\0') return -1; // 0 == Success, -1 == Fail

        if (isExistInList(name)) return -1;
        
        int fileDescriptor = OpenForReadWrite(name, 0);
        if (fileDescriptor == -1) return -1;
        
	    Close(fileDescriptor);
        return Unlink(name); 
    }

    char* CreateFileName(char *name)
    {
        if (!name) return NULL;

        char* string = new char[strlen(name) + 1];
        for (int i = 0; i <= strlen(name); i++) string[i] = name[i];

        return string;
    }

    bool isExistInList(char *name)
    {
        if (!name || name[0] == '\0') return 0;

        for (int i = 2; i < MAX_FILE; i++)
        if (bm->Test(i) && strcmp(name, filename[i]) == 0) return 1;

        return 0;
    }
};

#endif
