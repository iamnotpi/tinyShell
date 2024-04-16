#include <ctype.h>
#include <stdio.h>
#include <string.h>

#include "utils.h"
#include "../commands/commands.h"

bgProcess bgProcesses[MAX_BG_PROCESSES];
HANDLE hfgProcess;

void toLowerCase(char* str) {
    for(int i = 0; str[i]; i++){
        if (str[i] >= 'A' && str[i] <= 'Z') {
            str[i] = tolower(str[i]);
        }
    }
}

void executeCommand(int argc, char* argv[]) {
    if (argc == 0) return;
    for (int i = 0; i < argc; i++) {
        toLowerCase(argv[i]);
    }
    char* command = argv[0];
    if (strcmp(command, "&") == 0) {
        printf("^C\n");
        return;
    }
    for (int i = 0; commands[i].name != NULL; i++) {
        if (strcmp(command, commands[i].name) == 0) {
            commands[i].func(argc, argv);
            return;
        }
    }
    if (runPathCommand(argc, argv) == 0) {
        printf("'%s' is not recognized as an internal or external command, exe file or batch file. Type 'help' for the list of all internal supported commands.\n", command);
    }
}

void addPinfo(PROCESS_INFORMATION pi, char* status) {
    bgProcesses[bgCount].ProcessId = pi.dwProcessId;
    sprintf(bgProcesses[bgCount].bgPname, "child_process_%d", bgCount);
    strcpy(bgProcesses[bgCount].status, status);
    bgCount++;
}

int getPindex(DWORD ProcessId) {
    for (int i = 0; i < bgCount; i++) {
        if (bgProcesses[i].ProcessId == ProcessId) {
            return i;
        }
    }
    return -1;
}

int checkAndRemoveProcess(DWORD ProcessId) {
    int change = 0;
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, ProcessId);
    if (hProcess != NULL) {
        DWORD exitCode;
        if (GetExitCodeProcess(hProcess, &exitCode) && (exitCode != STILL_ACTIVE)) { 
            int index = getPindex(ProcessId);
            if (index == -1) return 0;
            for (int i = index; i < bgCount - 1; i++) {
                bgProcesses[i] = bgProcesses[i + 1];
            }
            bgCount--;
            change = 1;
        }
        CloseHandle(hProcess);
    }
    return change;
}

void suspend(HANDLE processHandle) {
    NtSuspendProcess pfnNtSuspendProcess = (NtSuspendProcess)GetProcAddress(
        GetModuleHandle("ntdll"), "NtSuspendProcess");
    pfnNtSuspendProcess(processHandle);
}

void resume(HANDLE processHandle) {
    NtResumeProcess pfnNtResumeProcess = (NtResumeProcess)GetProcAddress(
        GetModuleHandle("ntdll"), "NtResumeProcess");
    pfnNtResumeProcess(processHandle);
}

int isbatfile(int argc, char* argv[]) {
    if (argc == 0) return 0;
    char* str = argv[0];
    int len = strlen(argv[0]);
    if (len <= 4) return 0;
    if (str[len - 1] == 't' && str[len - 2] == 'a' && str[len - 3] == 'b' && str[len - 4] == '.') return 1;
    return 0;
}

int isexefile(int argc, char* argv[]) {
    if (argc == 0) return 0;
    char* str = argv[0];
    int len = strlen(argv[0]);
    if (len <= 4) return 0;
    if (str[len - 1] == 'e' && str[len - 2] == 'x' && str[len - 3] == 'e' && str[len - 4] == '.') return 1;
    return 0;
}

BOOL DirectoryExists(LPCTSTR szPath) {
    DWORD dwAttrib = GetFileAttributes(szPath);
    return (dwAttrib != INVALID_FILE_ATTRIBUTES && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

void initializeCustomPath() {
    const char* systemPath = getenv("PATH");
    if (systemPath != NULL) {
        strncpy(customPath, systemPath, sizeof(customPath) - 1);
        customPath[sizeof(customPath) - 1] = '\0'; // Ensure null termination
    }
}

void createP(char* filename, char* mode) {
    if (strcmp(mode, "bg") != 0 && strcmp(mode, "fg") != 0) {
        printf("Invalid mode. Expected fg or bg, but got %s instead.\n", mode);
        return;
    }
    STARTUPINFO si;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    PROCESS_INFORMATION pi;
    ZeroMemory(&pi, sizeof(pi));

    if(!CreateProcess(filename, NULL, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi)) {
        printf("Create process failed. Error: %d\n", GetLastError());
        return;
    }

    if (strcmp(mode, "bg") == 0) {
        bgProcesses[bgCount].ProcessId = pi.dwProcessId;
        sprintf(bgProcesses[bgCount].bgPname, "child_process_%d", bgCount);
        strcpy(bgProcesses[bgCount].status, "RUNNING");
        bgCount++;
        WaitForSingleObject(pi.hProcess, 0);
    }
    else if (strcmp(mode, "fg") == 0) {
        hfgProcess = pi.hProcess;
        WaitForSingleObject(pi.hProcess, INFINITE);
    }
}