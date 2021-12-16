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

	// ���� �ʱ�ȭ
	memset(copyCommandLine, 0, sizeof(copyCommandLine));
	memcpy(copyCommandLine, commandLine, sizeOfCommand);

	// ����� ���� ��ūȭ
	INT cnt = 0;
	WCHAR* context = NULL;

	pStrBase = wcstok_s(copyCommandLine, L" \r\n", &context);
	while (pStrBase != NULL)
	{
		if (cnt > (MAX_COMMAND_DATA - 1))	break;

		// ����� ����
		errno_t e = wcscpy_s(commandData[cnt], wcslen(pStrBase) + 1, pStrBase);
		if (e != NULL)
		{
			wprintf(L"wcscpy_s error\n");
			return false;
		}
		cnt++;

		// ���� ����� �̵�
		pStrBase = wcstok_s(NULL, L" ", &context);
	}
	// ����� ���ڿ� ���� ����
	*commLineCount = cnt;

	return true;
}