#pragma once

#include <phnt.h>

#define BUFFER_SIZE 1024

extern HANDLE ParentStdout;

extern HANDLE ChildStdinR;
extern HANDLE ChildStdinW;
extern HANDLE ChildStdoutR;
extern HANDLE ChildStdoutW;

BOOL CreatePipes();
VOID ReadFromPipe();
VOID WriteToPipe();