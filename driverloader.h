#pragma once
#include <Windows.h>

BOOL installDvr(CONST WCHAR drvPath[50], CONST WCHAR serviceName[20]);

BOOL startDvr(CONST WCHAR serviceName[20]);

BOOL stopDvr(CONST WCHAR serviceName[20]);

BOOL unloadDvr(CONST WCHAR serviceName[20]);