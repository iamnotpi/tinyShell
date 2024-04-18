# TinyShell
## Description
My implementation of a simple `tinyshell` based on Win32API.
## Getting started
To run `tinyshell`, open the terminal, navigate to the folder and compile the program using
```
gcc -o tinyshell tinyshell.c commands/commands.c utils/utils.c
gcc -o foo.c 
```
And then you can run `tinyshell.exe`. <br>
**Note:** Compiling `foo.c` is optional, but if you want to run  `child` command without any other executable, it's compulsory to compile `foo.c` since it is the default executable for the `child` command.
## Commands
`tinyshell` supports the following commands:
- `filename [args, optional]`: Execute the given file with the following arguments, if any. `tinyshell` supports running 2 file types: `.bat` and `.exe`
- `addpath`: Add a new path to the PATH environment variable. Note that `tinyshell` only inherits the system's PATH, using this command only affects the shell's path without adding anything to the system's PATH.
- `child [-bg, optional] [args, optional]`: Start a child process. If the argument `-bg` is provided as the second argument, start all of the specified processes in background mode. If not, start the **first** specifed process only in foreground mode. If no `args` is provided, start `foo.exe` in the appropriate mode.
- `clear`: Clear the terminal screen.
- `date`: Get current date.
- `dir`: Directory listing.
- `exit`: Kill all background processes, clear the screen and exit the shell.
- `help [args, optional]`: Provide help information. If `args` is not `NULL`, provide help information for the first command.
- `kill [args, optional]`: Kill all background processes. If `args` is not `NULL`, kill all specified processes, if exist. When encounter a non-exist process, `kill` terminates.
- `list`: List all working background processes with their IDs and status.
- `path`: Get the shell's path. **Note**: This path only inherits the system's PATH, modify this will not cause any change to the system PATH.
- `resume [args, optional]`: Resume all background processes. If `args` is not `NULL`, resume all specified processes, if exist. When encounter a non-exist process, `resume` terminates.
- `resume [args, optional]`: Stop all background processes. If `args` is not `NULL`, stop all specified processes, if exist. When encounter a non-exist process, `stop` terminates.
- `time`: Get current time.



