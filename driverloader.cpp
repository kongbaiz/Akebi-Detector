#include "driverloader.h"

BOOL installDvr(CONST WCHAR drvPath[50], CONST WCHAR serviceName[20]) {
    SC_HANDLE schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    if (schSCManager == NULL) {
        CloseServiceHandle(schSCManager);
        return FALSE;
    }
    SC_HANDLE schService = CreateService(schSCManager, serviceName, serviceName, SERVICE_ALL_ACCESS, SERVICE_KERNEL_DRIVER, SERVICE_DEMAND_START, SERVICE_ERROR_IGNORE, drvPath, NULL, NULL, NULL, NULL, NULL);
    if (schService == NULL) {
        CloseServiceHandle(schService);
        CloseServiceHandle(schSCManager);
        return FALSE;
    }
    CloseServiceHandle(schService);
    CloseServiceHandle(schSCManager);
    return TRUE;
}


BOOL startDvr(CONST WCHAR serviceName[20]) {
    SC_HANDLE schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    if (schSCManager == NULL) {
        CloseServiceHandle(schSCManager);
        return FALSE;
    }
    SC_HANDLE hs = OpenService(schSCManager, serviceName, SERVICE_ALL_ACCESS);
    if (hs == NULL) {
        CloseServiceHandle(hs);
        CloseServiceHandle(schSCManager);
        return FALSE;
    }
    if (StartService(hs, 0, 0) == 0) {
        CloseServiceHandle(hs);
        CloseServiceHandle(schSCManager);
        return FALSE;
    }
    CloseServiceHandle(hs);
    CloseServiceHandle(schSCManager);
    return TRUE;
}


BOOL stopDvr(CONST WCHAR serviceName[20]) {
    SC_HANDLE schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    if (schSCManager == NULL) {
        CloseServiceHandle(schSCManager);
        return FALSE;
    }
    SC_HANDLE hs = OpenService(schSCManager, serviceName, SERVICE_ALL_ACCESS);
    if (hs == NULL) {
        CloseServiceHandle(hs);
        CloseServiceHandle(schSCManager);
        return FALSE;
    }
    SERVICE_STATUS status;
    if (QueryServiceStatus(hs, &status) == 0) {
        CloseServiceHandle(hs);
        CloseServiceHandle(schSCManager);
        return FALSE;
    }
    if (status.dwCurrentState != SERVICE_STOPPED && status.dwCurrentState != SERVICE_STOP_PENDING) {
        if (ControlService(hs, SERVICE_CONTROL_STOP, &status) == 0) {
            CloseServiceHandle(hs);
            CloseServiceHandle(schSCManager);
            return FALSE;
        }
        INT timeOut = 0;
        while (status.dwCurrentState != SERVICE_STOPPED) {
            timeOut++;
            QueryServiceStatus(hs, &status);
            Sleep(50);
        }
        if (timeOut > 80) {
            CloseServiceHandle(hs);
            CloseServiceHandle(schSCManager);
            return FALSE;
        }
    }
    CloseServiceHandle(hs);
    CloseServiceHandle(schSCManager);
    return TRUE;
}


BOOL unloadDvr(CONST WCHAR serviceName[20]) {
    SC_HANDLE schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    if (schSCManager == NULL) {
        CloseServiceHandle(schSCManager);
        return FALSE;
    }
    SC_HANDLE hs = OpenService(schSCManager, serviceName, SERVICE_ALL_ACCESS);
    if (hs == NULL) {
        CloseServiceHandle(hs);
        CloseServiceHandle(schSCManager);
        return FALSE;
    }
    if (DeleteService(hs) == 0) {
        CloseServiceHandle(hs);
        CloseServiceHandle(schSCManager);
        return FALSE;
    }
    CloseServiceHandle(hs);
    CloseServiceHandle(schSCManager);
    return TRUE;
}