#ifndef UTILS_H  
#define UTILS_H  

#include <windows.h>

#define MAX_BG_PROCESSES 256
#define MAX_FG_PROCESSES 256

void toLowerCase(char* str);
int isbatfile(int argc, char* argv[]);
int isexefile(int argc, char* argv[]);
void addPinfo(PROCESS_INFORMATION pi, char* status);
int getPindex(DWORD ProcessId);
int checkAndRemoveProcess(DWORD ProcessId);
void executeCommand(int argc, char* argv[]);
void suspend(HANDLE processHandle);
void resume(HANDLE processHandle);
void initializeCustomPath();
BOOL DirectoryExists(LPCTSTR szPath);
void createP(char* filename, char* mode);

typedef struct {
    char bgPname[256];
    DWORD ProcessId;
    char status[10]; 
} bgProcess;

typedef LONG (NTAPI *NtSuspendProcess)(IN HANDLE ProcessHandle);
typedef LONG (NTAPI *NtResumeProcess)(IN HANDLE ProcessHandle);

extern bgProcess bgProcesses[MAX_BG_PROCESSES];
extern HANDLE hfgProcess;
extern int bgCount;

#endif