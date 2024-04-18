# TinyShell
## Description
My implementation of a simple `tinyshell` based on Win32API.
## Getting started
To run `tinyshell`, open the terminal, navigate to the folder and compile the program you using
```
gcc -o tinyshell tinyshell.c commands/commands.c utils/utils.c
gcc -o foo.c 
```
And then you can run `tinyshell.exe`. <br>
**Note:** Compiling `foo.c` is optional, but if you want to run  `child` command without any other executable, it's compulsory to compile `foo.c` since it is the default executable for the `child` command.
## Commands
`tinyshell` supports the following commands:


