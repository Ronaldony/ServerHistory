#include <stdio.h>
#include <windows.h>

#pragma warning(disable:4996)

#define BUF_SIZE 1024
int CommToClinet(HANDLE);

int wmain(int argc, WCHAR* argv[])
{
	CONST WCHAR* pipeName = L"\\\\.\\pipe\\simple_pipe";
	HANDLE hPipe;

	while(1)
	{
		hPipe = CreateNamedPipe(
			pipeName,
			PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED, // FILE_FLAG_OVERLAPPED: 비동기 I/O 옵션
			PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
			PIPE_UNLIMITED_INSTANCES,
			BUF_SIZE / 2,
			BUF_SIZE / 2,
			20000,
			NULL
		);

		if (hPipe == INVALID_HANDLE_VALUE)
		{
			wprintf(L"CreateNamedPipe failed\n");
			return -1;
		}

		bool isSuccess;
		isSuccess = ConnectNamedPipe(hPipe, NULL) ? TRUE : (GetLastError() == ERROR_PIPE_CONNECTED);
		
		if (isSuccess)
			CommToClinet(hPipe);
		else
			CloseHandle(hPipe);
	}
	return 1;	

	return 0;
}

int CommToClinet(HANDLE hPipe)
{
	WCHAR fileName[MAX_PATH];
	WCHAR dataBuf[BUF_SIZE];
	bool isSuccess;
	DWORD fileNameSize;

	isSuccess = ReadFile(
		hPipe, fileName, MAX_PATH * sizeof(WCHAR),
		&fileNameSize, NULL);

	if (!isSuccess || fileNameSize == 0)
	{
		wprintf(L"Pipe read message error!\n");
		return -1;
	}

	FILE* filePtr = _wfopen(fileName, L"r");
	if (filePtr == nullptr)
	{
		wprintf(L"_wfopen error! %s %d\n", fileName, fileNameSize);
		return -1;
	}

	OVERLAPPED overlappedInst;

	memset(&overlappedInst, 0, sizeof(overlappedInst));
	overlappedInst.hEvent = CreateEvent(
		NULL,	// 보안 속성
		TRUE,	// 수동 리셋
		TRUE,	// 초기 상태 signaled
		NULL	// 이름없는 오브젝트
	);

	DWORD bytesWritten = 0, bytesRead = 0;
	DWORD bytesWrite = 0, bytesTransfer = 0;
	
	while (!feof(filePtr))
	{
		bytesRead = fread(dataBuf, 1, BUF_SIZE, filePtr);
		bytesWrite = bytesRead;

		// 비동기 옵션 파이프와 OVERLAPPED 구조체 인자로 넣기
		isSuccess = WriteFile(hPipe, dataBuf, bytesWrite, &bytesWritten, &overlappedInst);
		
		if (!isSuccess && GetLastError() != ERROR_IO_PENDING)
		{
			wprintf(L"Pipe write message error\n");
			break;
		}

		// 위 WriteFile 함수가 파이프 통신 I/O 작업을 하는 동안 실행 할 코드

		WaitForSingleObject(overlappedInst.hEvent, INFINITE);
		GetOverlappedResult(hPipe, &overlappedInst, &bytesTransfer, FALSE);
		wprintf(L"Transferred data size: %u \n", bytesTransfer);
	}
	FlushFileBuffers(hPipe);
	DisconnectNamedPipe(hPipe);
	CloseHandle(hPipe);
	return 1;
}
