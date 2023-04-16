#include "syscall.h"

#define MAX_LENGTH 255

int main()
{
    int pingPID, pongPID;
    Write("Ping-Pong test starting... \n\n", MAX_LENGTH, ConsoleOutput);
    pingPID = Exec("./test/ping");
    pongPID = Exec("./test/pong");
    
    while(1);
}
