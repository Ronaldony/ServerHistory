#include <Windows.h>
#include <iostream>
#include <string.h>

using namespace std;

#define BUF_SIZE 1024

INT wmain(INT argc, WCHAR* argv[])
{
	HANDLE hPipe;
	WCHAR readDataBuf[BUF_SIZE + 1] = { 0, };
	WCHAR pipeName[] = L"\\\\.\\pipe\\simple_pipe";

	while (1)
	{
		/* 파이프에 연결 요청 */
		hPipe = CreateFile(
			pipeName,
			GENERIC_READ | GENERIC_WRITE,
			0,
			NULL,
			OPEN_EXISTING,
			0,
			NULL
			);

		/* 오류 검사 */
		if (hPipe != INVALID_HANDLE_VALUE)				// 핸들의 값이 무효한 경우
			break;
		if (GetLastError() != ERROR_PIPE_BUSY)			// PIPE가 바쁘지 않은 경우. 이 상황은 PIPE가 바쁘지 않음에도 불구하고 위 조건 핸들의 값이 무효하여 함수 호출간 문제가 발생하여 프로그램을 종료함
		{
			wprintf(L"Could Not open pipe\n");
			return 0;
		}

		if (!WaitNamedPipe(pipeName, 20000))	// WaitNamedPipe(): 연결 가능한 상태가 되면 TRUE를 반환한다.
		{
			wprintf(L"Could not open pipe\n");
			return 0;
		}
	}

	/* 파이프 속성 설정 */
	DWORD pipeMode = PIPE_READMODE_MESSAGE | PIPE_WAIT;	// 메시지 기반 모드
	BOOL isSuccess = SetNamedPipeHandleState(
		hPipe,			// 서버 파이프와 연결된 핸들
		&pipeMode,		// 변경할 모드 정보
		NULL,
		NULL);

	if (!isSuccess)		// 오류 검사.
	{
		wprintf(L"SetNamedPipeHandleState failed!\n");
		return 0;
	}

	/* 파이프에 데이터 전송 */
	CONST WCHAR* fileName = L"news.txt";
	DWORD bytesWritten = 0;

	isSuccess = WriteFile(
		hPipe,
		fileName,
		(wcslen(fileName) + 1) * sizeof(WCHAR),		// _tcslen는 유니코드든 MBCS든 문자의 길이만큼만 반환한다.
		&bytesWritten,
		NULL);

	wprintf(L"bytesWritten: %d. sizeof: %d\n", bytesWritten, (wcslen(fileName) + 1) * sizeof(WCHAR));
	if (!isSuccess)
	{
		wprintf(L"WriteFile error!\n");
		return 0;
	}

	DWORD bytesRead = 0;

	while (1)
	{
		isSuccess = ReadFile(
			hPipe,
			readDataBuf,
			BUF_SIZE * sizeof(WCHAR),
			&bytesRead,
			NULL);

		if ((!isSuccess) && (GetLastError() != ERROR_MORE_DATA))
		{
			break;
		}

		readDataBuf[bytesRead] = '\0';
		wprintf(L"%s \n", readDataBuf);
	}

	getchar();

	return 0;
}
