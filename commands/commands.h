#ifndef COMMANDS_H  // Check if COMMANDS_H has not been defined yet
#define COMMANDS_H  // Define COMMANDS_H

void addpathCommand(int arg, char* argv[]);
void childprocess(int argc, char* argv[]);
void clearCommand(int argc, char* argv[]);
void exitCommand(int argc, char* argv[]);
void helpCommand(int argc, char* argv[]);
void killCommand(int argc, char* argv[]);
void listCommand(int argc, char* argv[]);
void dirCommand(int argc, char* argv[]);
void dateCommand(int argc, char* argv[]);
void resumeCommand(int argc, char* argv[]);
void stopCommand(int argc, char* argv[]);
void pathCommand(int argc, char* argv[]);
void timeCommand(int argc, char* argv[]);
void runbatCommand(int argc, char* argv[]);
void runexeCommand(int argc, char* argv[]);
int runPathCommand(int argc, char* argv[]);

typedef struct {
    const char* name;
    const char* description;
    const char* detailed_description;
    void (*func)(int argc, char* argv[]);
} cmd;

extern cmd commands[];
extern char customPath[8192];

#endif