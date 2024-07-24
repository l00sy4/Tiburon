#include <Pipes.h>
#include <iostream>
#include <sstream>

HANDLE ParentStdout{ GetStdHandle(STD_OUTPUT_HANDLE) };

HANDLE ChildStdinR{ NULL };
HANDLE ChildStdinW{ NULL };
HANDLE ChildStdoutR{ NULL };
HANDLE ChildStdoutW{ NULL };

BOOL CreatePipes()
{	
	SECURITY_ATTRIBUTES PipeAttributes = { sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };

	if (!CreatePipe(&ChildStdoutR, &ChildStdoutW, &PipeAttributes, NULL))
	{
		return FALSE;
	}

	if (!SetHandleInformation(ChildStdoutR, HANDLE_FLAG_INHERIT, NULL))
	{
		return FALSE;
	}
	
	if (!CreatePipe(&ChildStdinR, &ChildStdinW, &PipeAttributes, NULL))
	{
		return FALSE;
	}

	if (!SetHandleInformation(ChildStdinW, HANDLE_FLAG_INHERIT, NULL))
	{
		return FALSE;
	}

	if (!ChildStdinR || !ChildStdinW || !ChildStdoutR || !ChildStdoutW)
	{
		return FALSE;
	}

	return TRUE;
}

VOID WriteToPipe()
{	
	while (TRUE)
	{	
		std::string Command;
		std::getline(std::cin, Command);

		Command += '\n';

		DWORD BytesWritten{};

		if (!WriteFile(ChildStdinW, Command.c_str(), Command.length(), &BytesWritten, NULL))
		{
			break;
		}
	}
}

VOID ReadFromPipe()
{	
	while (TRUE)
	{	
		CHAR Buffer[BUFFER_SIZE];
		DWORD BytesRead{};

		if (!ReadFile(ChildStdoutR, Buffer, BUFFER_SIZE, &BytesRead, NULL))
		{
			break;
		}
	
		DWORD BytesWritten{};

		if (!WriteFile(ParentStdout, Buffer, BytesRead, &BytesWritten, NULL))
		{
			break;
		}
	}
}