#include <stdio.h>
#include <string.h>
#include <windows.h>
#include "define.h"
#include "extern.h"
#include "global.h"

bool tokenSTR(CONST WCHAR* commandLine, CONST INT sizeOfCommand, WCHAR (*commandData)[MAX_COMMAND_LEN], INT* commLineCount)
{
	WCHAR copyCommandLine[MAX_COMMAND_LEN];
	WCHAR* pStrBase;

	// 버퍼 초기화
	memset(copyCommandLine, 0, sizeof(copyCommandLine));
	memcpy(copyCommandLine, commandLine, sizeOfCommand);

	// 명령행 문자 토큰화
	INT cnt = 0;
	WCHAR* context = NULL;

	pStrBase = wcstok_s(copyCommandLine, L" \r\n", &context);
	while (pStrBase != NULL)
	{
		if (cnt > (MAX_COMMAND_DATA - 1))	break;

		// 명령행 복사
		errno_t e = wcscpy_s(commandData[cnt], wcslen(pStrBase) + 1, pStrBase);
		if (e != NULL)
		{
			wprintf(L"wcscpy_s error\n");
			return false;
		}
		cnt++;

		// 다음 명령행 이동
		pStrBase = wcstok_s(NULL, L" ", &context);
	}
	// 명령행 문자열 개수 저장
	*commLineCount = cnt;

	return true;
}