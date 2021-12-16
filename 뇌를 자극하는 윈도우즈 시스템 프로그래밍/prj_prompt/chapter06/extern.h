#ifndef __EXTERN_H__
#define __EXTERN_H__

// lib.cpp
extern bool tokenSTR(CONST WCHAR*, CONST INT, WCHAR (*)[MAX_COMMAND_LEN], INT*);

// function.cpp
extern bool CommandProcess(CONST WCHAR(*commandLine)[MAX_COMMAND_LEN], CONST INT commLineCnt);
extern INT CommandNumber(CONST WCHAR* comm);
extern void _command_start(CONST WCHAR(*commandLine)[MAX_COMMAND_LEN], CONST INT commLineCnt);
extern void _command_program(WCHAR* command);

#endif