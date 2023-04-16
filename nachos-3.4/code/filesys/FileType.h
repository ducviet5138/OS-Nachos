#ifndef FILETYPE_H
#define FILETYPE_H
#include "openfile.h"

#define MAX_FILE 10
#define READWRITE 0
#define READONLY 1
#define MAX_LENGTH 255

// =========================================================
class NewOpenFile: public OpenFile
{
public:
    NewOpenFile(int sector): OpenFile(sector){}
    
    int Read(char *buffer, int count)
    {
        if (count < 0 || count > MAX_LENGTH || !buffer) return -1;

        OpenFile::Read(buffer, count);

        return strlen(buffer);
    }

    virtual int Write(char *buffer, int count){}
};

// =========================================================
class ReadOnly: public NewOpenFile
{
public:
    ReadOnly(int sector):NewOpenFile(sector){}

    int Write(char *buffer, int count)
    {
        return -1; // Cannot write in READONLY file
    }
};

// =========================================================
class ReadWrite: public NewOpenFile
{
public:
    ReadWrite(int sector):NewOpenFile(sector){}

    int Write(char *buffer, int count)
    {
        if (count < 0 || !buffer) return -1;
        if (count == 0 || buffer[0] == '\0') return 0;

        int RealSize = (strlen(buffer) < count) ? strlen(buffer) : count;
        OpenFile::Write(buffer, RealSize);

        return RealSize;
    }
};

#endif