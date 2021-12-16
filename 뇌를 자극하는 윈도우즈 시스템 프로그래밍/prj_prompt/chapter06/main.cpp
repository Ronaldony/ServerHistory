#include <stdio.h>
#include <string.h>
#include <windows.h>
#include "define.h"
#include "extern.h"
#include "global.h"

INT wmain(INT argc, WCHAR* argv[])
{
	WCHAR commandLine[MAX_COMMAND_LEN] = { 0, };
	WCHAR commandData[MAX_COMMAND_DATA][MAX_COMMAND_LEN] = { 0, };
	INT commLineCnt;

	wprintf(L"argc: %d\n", argc);
	if (argc >= 2)
	{
		// 명령행 인자 문자열 복사
		for (INT cnt = 1; cnt < argc; cnt++)
			wcscpy_s(commandData[cnt - 1], wcslen(argv[cnt]) + 1, argv[cnt]);	// + 1 NULL 포함

		// 명령행
		CommandProcess((CONST WCHAR(*)[MAX_COMMAND_LEN])commandData, argc - 1);
	}

	while (1)
	{
		// 버퍼 초기화
		rewind(stdin);
		memset(commandLine, 0, sizeof(commandLine));
		memset(commandData, 0, sizeof(commandData));

		wprintf(L"command>>");
		fgetws(commandLine, sizeof(commandLine) / sizeof(WCHAR), stdin);
		_wcslwr_s(commandLine, sizeof(commandLine) / sizeof(WCHAR));

		// 문자 토큰화하여 문자저장
		if (tokenSTR((CONST WCHAR*)commandLine, sizeof(commandLine), (WCHAR(*)[MAX_COMMAND_LEN])commandData, &commLineCnt) == false)
			break;

		// 명령행
		if (CommandProcess((CONST WCHAR(*)[MAX_COMMAND_LEN])commandData, commLineCnt) == false)
			break;
	}
}