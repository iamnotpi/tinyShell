#include "commands.h"
#include "../utils/utils.h"

#include <windows.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

int bgCount = 0;
int fgCount = 0;
char customPath[8192] = "";

cmd commands[] = {
    {"<batch_file.bat> [args]", "Execute the given .bat file with the following arguments, if any.", "<batch_file.bat> [args]: Execute the given .bat file with the following arguments, if any.", runbatCommand},
    {"<exe_file.exe> [args]", "Execute the given .exe file with the following arguments, if any.", "<exe_file.exe> [args]: Execute the given .exe file with the following arguments, if any.", runexeCommand},
    {"addpath", "Add a new path to the PATH environment variable.", "addpath <path>: Add a new path to the path environment variable.", addpathCommand},
    {"child", "Start a simple child process.", "child [args] : Start child process(es). \nIf the argument '-bg' is provided as the second argument (i.e. 'child -bg ...'), start all specified process in background mode.\nIf '-bg' is not provided, start the first process appears in the command line in foreground mode.\nNote: 'child' can start multiple (non-UWP) .exe file by using 'child -bg <file1.exe> <file2.exe>....\nBy default, if there is no argument provided, 'child' starts a simple foo.exe process (associated with foo.c) in foreground mode. If only '-bg' argument is provided, child starts the foo.exe in background mode.", childprocess},
    {"clear", "Clear the terminal screen.", "clear        : Clear the terminal screen.", clearCommand},
    {"date", "Get current date.", "date         : Get current date.", dateCommand},
    {"dir", "Directory listing.", "dir          : Directory listing.", dirCommand},
    {"exit", "Exit the shell.", "exit         : Exit the shell. \nNote: This command also clears the screen and kills all background processes.", exitCommand},
    {"help", "Provide help information.", "help [args]  : Provide help information.\nIf there are any arguments provided, provide help information for the first command argument.", helpCommand},
    {"kill", "Kill all background processes.", "kill   [args]: Kill all background processes.\nIf there are any arguments provided, kill all background processes that have the same name as the arguments provided.\nIf one of the name provided is incorrect, terminate this command without killing any processes that appear later in the command line arguments.", killCommand},
    {"list", "List all working background processes with their IDs and status.", "list        : List all working background processes with their IDs and status.", listCommand},
    {"path", "Get the system path.", "path         : Get the system path.", pathCommand},
    {"resume", "Resume background processes.", "resume [args]: Resume background processes.\nIf there are any arguments provided, resume all background processes that have the same name as the arguments provided.\nIf one of the name provided is incorrect, terminate this command without resuming any processes that appear later in the command line arguments.", resumeCommand},
    {"stop", "Stop background processes.", "stop [args]  : Stop background processes.\nIf there are any arguments provided, stop all background processes that have the same name as the arguments provided.\nIf one of the name provided is incorrect, terminate this command without stopping any processes that appear later in the command line arguments.", stopCommand},
    {"time", "Get current time.", "time         : Get current time.", timeCommand},
    {NULL, NULL, NULL, NULL}
};

void helpCommand(int argc, char* argv[]) {
    if (argc > 2) {
        printf("Provides help information for all supported commands.\n\n");
        printf("Type: help [command] for help information on that command.\n");
    }
    else if (argc == 1) {
        printf("Supported commands:\n");
        for (int i = 0; commands[i].name != NULL; i++) {
            printf("%-25s: %s\n", commands[i].name, commands[i].description);
        }
        printf("For more detail about a command, type 'help <command_name>'.\n");
    }
    else if (argc == 2) {
        char* help_command = argv[1];
        for (int i = 0; commands[i].name != NULL; i++) {
            if (strcmp(help_command, commands[i].name) == 0) {
                printf("%s\n", commands[i].detailed_description);
                return;
            }
        }
        printf("'%s' is not recognized as an internal or external command, exe file or batch file. Type 'help' for the list of all internal supported commands.\n", help_command);
    }
}

void exitCommand(int argc, char* argv[]) {
    killCommand(argc, argv);
    system("cls");
    exit(0);
}

void dirCommand(int argc, char* argv[]) {
    // About WIN32_FIND_DATA: https://learn.microsoft.com/en-us/windows/win32/api/minwinbase/ns-minwinbase-win32_find_dataa
    WIN32_FIND_DATA findData;

    // About HANDLE: https://stackoverflow.com/questions/902967/what-is-a-windows-handle
    // About FindFirstFile: https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-findfirstfilea
    HANDLE hFind = FindFirstFile("./*", &findData);

    if (hFind == INVALID_HANDLE_VALUE) {
        printf("Failed to open directory.\n");
        return;
    }

    do {
        printf("%s\n", findData.cFileName);
    } 
    while (FindNextFile(hFind, &findData) != 0);

    FindClose(hFind);
}

void dateCommand(int argc, char* argv[]) {
    time_t rawtime;
    struct tm * timeinfo;

    // An array to hold the formatted time string
    char buffer[80];

    // Get current time in seconds since the Unix Epoch and store it in rawtime
    time(&rawtime);

    // Convert the time in rawtime to broken-down time and store the result in timeinfo
    timeinfo = localtime(&rawtime);

    // Format the time in timeinfo as a string and stores it in buffer.
    strftime(buffer, sizeof(buffer),"%Y-%m-%d", timeinfo);
    printf("Current date: %s\n", buffer);    
}

void timeCommand(int argc, char* argv[]) {
    time_t rawtime;
    struct tm * timeinfo;
    char buffer[80];

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(buffer, sizeof(buffer),"%H:%M:%S", timeinfo);
    printf("Current time: %s\n", buffer);    
}

void runbatCommand(int argc, char* argv[]) {
    char* bat_filename = argv[0];
    STARTUPINFO si; // A structure that specifies the window station,
                    // desktop, standard handles, and appearance of the main window for a 
                    // process at creation time.
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si); // Size of the structure, in bytes
    PROCESS_INFORMATION pi; // A structure that contains information about a newly created
                            // process and its primary thread.
    ZeroMemory(&pi, sizeof(pi));

    // About lpApplicationName (1st parameter in CreateProcess):
    // The lpApplicationName parameter can be NULL. 
    // In that case, the module name must be the 
    // first white space–delimited token in the lpCommandLine string.

    // To run a batch file, start the command interpreter; 
    // set lpApplicationName to cmd.exe 
    // and set lpCommandLine to /c plus the name of the batch file
    char cmdLine[10000] = "cmd.exe /c ";
    for (int i = 0; i < argc; i++) {
        strcat(cmdLine, argv[i]);
        strcat(cmdLine, " ");
    }
    if (!CreateProcess(NULL,           // No module name (use command line)
                       cmdLine,        // Command line
                       NULL,           // Process handle not inheritable
                       NULL,           // Thread handle not inheritable
                       FALSE,          // Set handle inheritance to FALSE
                       0,              // No creation flags
                       NULL,           // Use parent's environment block
                       NULL,           // Use parent's starting directory 
                       &si,            // Pointer to STARTUPINFO structure
                       &pi)            // Pointer to PROCESS_INFORMATION structure
    ) 
    {
        printf("CreateProcess failed (%d).\n", GetLastError());
    }

    // Wait until child process exits.
    WaitForSingleObject(pi.hProcess, INFINITE);

    // Close process and thread handles. 
    CloseHandle(pi.hProcess); 
    CloseHandle(pi.hThread); 
}

void runexeCommand(int argc, char* argv[]) {
    char* exe_filename = argv[0];
    STARTUPINFO si; 
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si); 
    PROCESS_INFORMATION pi; 
    ZeroMemory(&pi, sizeof(pi));
    char cmdLine[10000] = "";
    for (int i = 0; i < argc; i++) {
        strcat(cmdLine, argv[i]);
        strcat(cmdLine, " ");
    }
    if (!CreateProcess(NULL, cmdLine, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
        printf("CreateProcess failed (%d).\n", GetLastError());
    }

    // Wait until child process exits.
    WaitForSingleObject(pi.hProcess, INFINITE);

    // Close process and thread handles. 
    CloseHandle(pi.hProcess); 
    CloseHandle(pi.hThread); 
}

void listCommand(int argc, char* argv[]) {
    if (bgCount == 0) {
        printf("No background process is working at the moment.\n");
        return;
    }

    int change;
    do {
        change = 0;
        int tmp = bgCount;
        for (int i = 0; i < tmp; i++) {
            if (checkAndRemoveProcess(bgProcesses[i].ProcessId)) {
                change = 1;
            }
        }
    } while (change);
  
    printf("Current working background processes: \n\n");
    printf("Process name     |PID   |Status   \n");
    printf("----------------------------------\n");
    for (int i = 0; i < bgCount; i++) {
        bgProcess bgP = bgProcesses[i];
        printf("%-17s|%-6d|%s\n", bgP.bgPname, bgP.ProcessId, bgP.status);
    }
}

void clearCommand(int argc, char* argv[]) {
    system("cls");
}

void killCommand(int argc, char* argv[]) {
    if (argc == 1) {
        // Kill all background processes
        for (int i = 0; i < bgCount; i++) {
            // OpenProcess returns an open handle to the specified process
            HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, // The access to the process object
                                                             // PROCESS_TERMINATE is required to terminate 
                                                             // a process using TerminateProcess.
                                          FALSE, // Will the processes created by this process inherit the handle?
                                          bgProcesses[i].ProcessId); // The identifier of the local process to be opened
            if (hProcess != NULL) {
                TerminateProcess(hProcess, 0);
                CloseHandle(hProcess);
            }
        }
        printf("All background processes killed.\n");
        bgCount = 0; // Reset the number of background processes to 0
    }
    else if (argc >= 2) {
        for (int i = 1; i < argc; i++) {
            int found = 0;
            for (int j = 0; j < bgCount; j++) {
                if (strcmp(argv[i], bgProcesses[j].bgPname) == 0) {
                    HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, bgProcesses[j].ProcessId);
                    if (hProcess != NULL) {
                        // Remove the process information from the bgProcesses list
                        int index = getPindex(bgProcesses[j].ProcessId);
                        for (int i = index; i < bgCount - 1; i++) {
                            bgProcesses[i] = bgProcesses[i + 1];
                        }
                        bgCount--;

                        // Terminate the process
                        TerminateProcess(hProcess, 0);
                        CloseHandle(hProcess);
                    }
                    found = 1;
                    break;
                }
            }
            if (!found) {
                printf("Invalid background process name: %s. Type 'list' for the list of all background processes.\n", argv[i]);
                return;
            }
        }
    }
}

void pathCommand(int argc, char* argv[]) {
    printf("PATH=%s\n", customPath);
}

int runPathCommand(int argc, char* argv[]) {
    char command[4096];
    char* path = strdup(customPath); // Duplicate the path because strtok modifies the string
    char* dir = strtok(path, ";");
    while (dir != NULL) {
        // Construct the full path
        snprintf(command, sizeof(command), "%s\\%s", dir, argv[0]);

        STARTUPINFO si; 
        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si); 
        PROCESS_INFORMATION pi; 
        ZeroMemory(&pi, sizeof(pi));

        if (CreateProcess(NULL, command, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
            CloseHandle(pi.hProcess); 
            CloseHandle(pi.hThread);
            free(path);
            return 1;
        }
        // Try the next directory
        dir = strtok(NULL, ";");
    }
    free(path);
    return 0;
}

void childprocess(int argc, char* argv[]) {
    if (argc == 1) createP("foo.exe", "fg");
    else if (argc == 2 && strcmp(argv[1], "-bg") == 0) createP("foo.exe", "bg");
    else if (argc > 2 && strcmp(argv[1], "-bg") == 0) {
        for (int i = 2; i < argc; i++) createP(argv[i], "bg");
    }
    else if (argc >= 2 && strcmp(argv[1], "-bg") != 0) createP(argv[1], "fg");
}

void stopCommand(int argc, char* argv[]) {
    if (argc == 1) {
        // Stop all background processes
        for (int i = 0; i < bgCount; i++) {
            if (strcmp(bgProcesses[i].status, "SUSPENDED") == 0) continue; 
            HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, bgProcesses[i].ProcessId); 
            if (hProcess != NULL) {
                suspend(hProcess);    
                strcpy(bgProcesses[i].status, "SUSPENDED"); 
            }
            CloseHandle(hProcess);
        }
        printf("All background processes have been stopped.\n");
    }
    else if (argc >= 2) {
        for (int i = 1; i < argc; i++) {
            int found = 0;
            for (int j = 0; j < bgCount; j++) {
                if (strcmp(argv[i], bgProcesses[j].bgPname) == 0) {
                    if (strcmp(bgProcesses[j].status, "SUSPENDED") != 0) {
                        HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, bgProcesses[j].ProcessId);
                        if (hProcess != NULL) {
                            suspend(hProcess);    
                            strcpy(bgProcesses[j].status, "SUSPENDED");
                            printf("%s has been stopped. To Stop, type 'resume %s'.\n", bgProcesses[j].bgPname, bgProcesses[j].bgPname);
                        }
                        CloseHandle(hProcess);
                    }
                    found = 1;
                    break;
                }
            }
            if (!found) {
                printf("Invalid background process name: %s. Type 'list' for the list of all background processes.\n", argv[i]);
                return;
            }
        }
    }
}

void resumeCommand(int argc, char* argv[]) {
    if (argc == 1) {
        // Resume all background processes
        for (int i = 0; i < bgCount; i++) {
            if (strcmp(bgProcesses[i].status, "RUNNING") == 0) continue; 
            HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, bgProcesses[i].ProcessId); 
            if (hProcess != NULL) {
                resume(hProcess);    
                strcpy(bgProcesses[i].status, "RUNNING");
            }
            CloseHandle(hProcess);
        }
        printf("All background processes have been resumed.\n");
    }
    else if (argc >= 2) {
        for (int i = 1; i < argc; i++) {
            int found = 0;
            for (int j = 0; j < bgCount; j++) {
                if (strcmp(argv[i], bgProcesses[j].bgPname) == 0) {
                    if (strcmp(bgProcesses[j].status, "RUNNING") != 0) {
                        HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, bgProcesses[j].ProcessId);
                        if (hProcess != NULL) {
                            resume(hProcess);    
                            strcpy(bgProcesses[j].status, "RUNNING");
                            printf("%s has been resumed. To stop, type 'stop %s'.\n", bgProcesses[j].bgPname, bgProcesses[j].bgPname);
                        }
                        CloseHandle(hProcess);
                    }
                    found = 1;
                    break;
                }
            }
            if (!found) {
                printf("Invalid background process name: %s. Type 'list' for the list of all background processes.\n", argv[i]);
                return;
            }
        }
    }
}

void addpathCommand(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: addpath <path>\n");
        return;
    }
    char* newPath = argv[1];
    if (DirectoryExists(newPath)) {
        if (customPath == NULL) strcpy(customPath, newPath);
        else {
            strcat(customPath, ";");
            strcat(customPath, newPath);
        }
        return;
    }
    printf("Cannot find the specified path directory. Please try again.\n");
}