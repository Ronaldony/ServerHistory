#include <stdio.h>
#include <string.h>
#include <windows.h>

#define MAIL_SLOT_NAME	(L"\\\\.\\mailslot\\mailbox")

INT wmain(INT argc, WCHAR* argv[])
{
	HANDLE hMailslot;		// 메일슬롯 핸들

	hMailslot = CreateMailslotW(
		MAIL_SLOT_NAME,			// 메일 슬롯 이름
		0,						// 최대 길이
		MAILSLOT_WAIT_FOREVER,	// 수신할 때 까지 블로킹
		NULL					// 보안 속성 미지정
	);

	// CreateMailslotW 함수 실패 시
	if (hMailslot == INVALID_HANDLE_VALUE)
	{
		wprintf(L"CreateMailslotW fail\n");
		return -1;
	}

	while (1)
	{
		WCHAR messageBox[50];	// 수신 버퍼
		DWORD bytesRead ;		// 읽은 버퍼 길이

		memset(messageBox, 0, sizeof(messageBox));

		bool isRead = ReadFile(
			hMailslot,				// 메일슬롯 핸들
			messageBox,				// 저장할 데이터 버퍼
			sizeof(messageBox),		// 읽을 데이터 수 
			&bytesRead,				// 읽은 데이터 수
			NULL					// 비동기 여부
		);

		// ReadFile 함수 오류
		if (isRead == false)
		{
			wprintf(L"ReadFile fail\n");
			CloseHandle(hMailslot);
			return -1;			
		}

		// 읽은 출력
		wprintf(L"Read buff: %s\n", messageBox);
		wprintf(L"Read bytes: %d\n", bytesRead);
	}
}
