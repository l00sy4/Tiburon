#include <FileIO.h>
#include <iostream>

LPVOID ReadPayload(_In_ HANDLE PayloadFile, _Inout_ PDWORD PayloadSize)
{
	*PayloadSize = GetFileSize(PayloadFile, NULL);

	if (*PayloadSize == INVALID_FILE_SIZE)
	{
		return NULL;
	}
	
	LPVOID PayloadBuffer = LocalAlloc(LMEM_FIXED, *PayloadSize);

	if (PayloadBuffer == NULL)
	{
		return NULL;
	}

	DWORD BytesRead{};

	if (!ReadFile(PayloadFile, PayloadBuffer, *PayloadSize, &BytesRead, NULL))
	{
		LocalFree(PayloadBuffer);
		return NULL;
	}

	return PayloadBuffer;
}

HANDLE CreateTempFile()
{

	WCHAR TempPath[MAX_PATH + 1];
	DWORD PathSize = GetTempPathW(MAX_PATH + 1, TempPath);

	if (PathSize > MAX_PATH + 1 || PathSize == 0)
	{
		return INVALID_HANDLE_VALUE;
	}

	WCHAR TempFileName[MAX_PATH];
	UINT TempFileID = GetTempFileNameW(TempPath, L"DD", 0, TempFileName);

	if (TempFileID == 0)
	{
		return INVALID_HANDLE_VALUE;
	}

	return CreateFileW(TempFileName, GENERIC_ALL, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
}