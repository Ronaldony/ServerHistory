#include <stdio.h>
#include <string.h>
#include <windows.h>

#define MAIL_SLOT_NAME	(L"\\\\.\\mailslot\\mailbox")

INT wmain(INT argc, WCHAR* argv[])
{
	HANDLE hMailslot;           // 메일슬롯 핸들

	hMailslot = CreateFile(
		MAIL_SLOT_NAME,                     // 메일슬롯 이름
		GENERIC_WRITE,                      // 파일 쓰기로 열기
		FILE_SHARE_READ | FILE_SHARE_WRITE, // 다른 프로세스와 Read, Write 파일 열기 방식으로 공유
		NULL,                               // 보안 속성
		OPEN_EXISTING,                      // 파일 열기 속성(파일이 존재하는 경우 파일 열기)
		FILE_ATTRIBUTE_NORMAL,              // ?
		NULL                                // ?
	);

	while (1)
	{
		WCHAR *context;
		WCHAR sendBuff[50];     // 전송 버퍼
		DWORD sendBytes;        // 전송한 바이트 크기

		memset(sendBuff, 0, sizeof(sendBuff));

		// 사용자 입력
		wprintf(L"User input>");
		fgetws(sendBuff, sizeof(sendBuff) / sizeof(WCHAR), stdin);
		wcstok_s(sendBuff, L"\r\n", &context);      // 개행 문자 삭제

		bool statusWrite = WriteFile(
			hMailslot,                          // 메일슬롯 핸들
			sendBuff,                           // 전송 버퍼
			wcslen(sendBuff) * sizeof(WCHAR),   // 전송할 버퍼 크기
			&sendBytes,                         // 전송된 버퍼 크기
			NULL                                // 보안 속성
		);

		if (statusWrite == false)
		{
			wprintf(L"WriteFile fail\n");
			CloseHandle(hMailslot);
			return -1;
		}
	}
}
