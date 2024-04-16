#include <ctype.h>
#include <stdio.h>
#include <string.h>

#include "commands/commands.h"
#include "utils/utils.h"

BOOL WINAPI consoleHandler(DWORD signal) {
    if (hfgProcess != NULL) {
        if (signal == CTRL_C_EVENT) {
            HANDLE hProcess = hfgProcess;
            if (hProcess != NULL) {
                TerminateProcess(hProcess, 0);
                CloseHandle(hProcess);
            } 
            else printf("Handle not opened. Error: %lu\n", GetLastError());
        }
    }
    return TRUE;
}

int main() {
    // Store all user input as a string
    char command[256];

    // Store each token in the user input as a seperate string
    char* command_arr[10];

    // Initialize the path environment variable
    char* customPath;
    initializeCustomPath();

    if (!SetConsoleCtrlHandler(consoleHandler, TRUE)) {
        printf("\nERROR: Could not set control handler"); 
        return 1;
    }

    while (1) {
        printf("MyShell>");
        fgets(command, 256, stdin);
        command[strcspn(command, "\n")] = 0; // Remove \n from the command string
        char* token = strtok(command, " ");
        int count = 0;
        while (token != NULL) {
            command_arr[count] = token;
            token = strtok(NULL, " ");
            count++;
        }
        if (count == 0) continue;
        else if (isbatfile(count, command_arr)) runbatCommand(count, command_arr);
        else if (isexefile(count, command_arr)) runexeCommand(count, command_arr);
        else executeCommand(count, command_arr);
    }
    return 0;
}