#pragma once

#include <phnt.h>

LPVOID ReadPayload(_In_ HANDLE PayloadFile, _Inout_ PDWORD PayloadSize);
HANDLE CreateTempFile();