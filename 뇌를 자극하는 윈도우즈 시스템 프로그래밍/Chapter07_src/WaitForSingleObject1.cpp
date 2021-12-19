#include <stdio.h>
#include <string.h>
#include <windows.h>

INT wmain(INT argc, WCHAR* argv[])
{
	WCHAR program[40] = L"WaitForSingleObject2.exe 10 20";
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	// 프로세스 생성 데이터 초기화
	ZeroMemory(&si, sizeof(si));
	ZeroMemory(&pi, sizeof(pi));

	si.cb = sizeof(si);
	// 자식 프로세스 생성
	BOOL state = CreateProcessW(
		NULL,
		program,
		NULL, NULL, TRUE,
		CREATE_NEW_CONSOLE, 
		NULL, NULL,
		&si, &pi
	);
	// CreateProcessW 함수 오류 처리
	if (state == false)
	{
		wprintf(L"CreateProcessW fail\n");
		return -1;
	}

	// 자식 프로세스 종료 대기
	WaitForSingleObject(pi.hProcess, INFINITE);
	// 종료 코드 얻기
	DWORD exitCode;
	if (GetExitCodeProcess(pi.hProcess, &exitCode) == false)
	{
		wprintf(L"GetExitCodeProcess fail\n");
		return -1;
	}
	// 결과 출력
	wprintf(L"ExitCode: %d\n", exitCode);
	wprintf(L"Rights: %d\n", 30 * 5);	

	// 일시정지
	rewind(stdin);
	getchar();

	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
}
