#include <stdio.h>
#include <string.h>
#include <windows.h>
#include "define.h"
#include "extern.h"
#include "global.h"

bool CommandProcess(CONST WCHAR(*commandLine)[MAX_COMMAND_LEN], CONST INT commLineCnt);
INT CommandNumber(CONST WCHAR* comm);
void _command_start(CONST WCHAR(*commandLine)[MAX_COMMAND_LEN], CONST INT commLineCnt);
void _command_program(WCHAR* command);

bool CommandProcess(CONST WCHAR (*commandLine)[MAX_COMMAND_LEN], CONST INT commLineCnt)
{
	INT procCommand;
	procCommand = CommandNumber(commandLine[0]);

	switch (procCommand)
	{
	case COMMAND_NUM_EXIT:		// 프로그램 종료
		wprintf(L"COMMAND_NUM_EXIT\n");
		return false;
		break;
	case COMMAND_NUM_START:		// "start" 명령
		wprintf(L"COMMAND_NUM_START\n");
		_command_start(commandLine, commLineCnt);
		break;
	case COMMAND_NUM_ECHO:		// "echo" 명령
		wprintf(L"COMMAND_NUM_ECHO\n");
		for (INT cnt = 1; cnt < commLineCnt; cnt++)
			wprintf(L"%s ", commandLine[cnt]);
		wprintf(L"\n");
		break;
	case COMMAND_NUM_PROGRAM:	// 입력 Program 실행
		wprintf(L"COMMAND_NUM_PROGRAM\n");
		_command_program((WCHAR*)commandLine[0]);
		break;
	default:
		wprintf(L"default\n");
	}

	return true;
}

// 설명: 입력받은 문자열을 명령 문자열과 비교하여 해당하는 명령 번호를 반환
INT CommandNumber(CONST WCHAR* comm)
{
	INT ret = COMMAND_NUM_PROGRAM;

	// 명령 문자 비교
	if (wcscmp(comm, L"exit") == NULL)
		ret = COMMAND_NUM_EXIT;
	else if (wcscmp(comm, L"start") == NULL)
		ret = COMMAND_NUM_START;
	else if (wcscmp(comm, L"echo") == NULL)
		ret = COMMAND_NUM_ECHO;

	return ret;
}

// 설명: "start" 명령 수행
void _command_start(CONST WCHAR(*commandLine)[MAX_COMMAND_LEN], CONST INT commLineCnt)
{

	if (commLineCnt >= 2)
	{
		WCHAR command[MAX_COMMAND_LEN] = L"Project.exe";

		// command 재조립
		for (INT cnt = 1; cnt < commLineCnt; cnt++)
			wsprintf(command, L"%s %s", command, commandLine[cnt]);

		wprintf(L"command: %s\n", command);
		// 프로세스 생성
		STARTUPINFO si;
		PROCESS_INFORMATION pi;

		ZeroMemory(&si, sizeof(si));
		ZeroMemory(&pi, sizeof(pi));

		si.cb = sizeof(si);

		bool state = CreateProcessW(
			NULL, command,
			NULL, NULL, TRUE,
			CREATE_NEW_CONSOLE, NULL, NULL,
			&si, &pi
		);

		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);
	}
	else
	{
		WCHAR command[MAX_COMMAND_LEN] = L"cmd.exe";
		// 프로세스 생성
		STARTUPINFO si;
		PROCESS_INFORMATION pi;

		ZeroMemory(&si, sizeof(si));
		ZeroMemory(&pi, sizeof(pi));

		si.cb = sizeof(si);

		bool state = CreateProcessW(
			NULL, command,
			NULL, NULL, TRUE,
			CREATE_NEW_CONSOLE, NULL, NULL,
			&si, &pi
		);

		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);
	}
}

// 설명: 명령이 아닌 프로그램 실행 명령문 수행
void _command_program(WCHAR* command)
{
	// 프로세스 생성
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	ZeroMemory(&si, sizeof(si));
	ZeroMemory(&pi, sizeof(pi));

	si.cb = sizeof(si);

	bool state = CreateProcessW(
		NULL, command,
		NULL, NULL, TRUE,
		CREATE_NEW_CONSOLE, NULL, NULL,
		&si, &pi
	);

	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);
}