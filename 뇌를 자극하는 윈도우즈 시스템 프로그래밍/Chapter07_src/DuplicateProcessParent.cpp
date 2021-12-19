#include <stdio.h>
#include <string.h>
#include <windows.h>

INT wmain(INT argc, WCHAR* argv[])
{
	// 핸들 복제
	WCHAR program[40] = L"DuplicateProcessChild.exe";	// 자식 프로세스 프로그램 이름
	HANDLE hThisProcess;
	
	DuplicateHandle(
		GetCurrentProcess(),	// 복제할 핸들 소유 프로세스
		GetCurrentProcess(),	// 복제할 핸들
		GetCurrentProcess(),	// 복제될 핸들 소유 프로세스
		&hThisProcess,			// 복제된 핸들 저장 주소
		0,
		TRUE,					// 상속여부 'Y'
		DUPLICATE_SAME_ACCESS
	);

	swprintf(program, sizeof(program) / sizeof(WCHAR), L"%s %d", program, (INT)hThisProcess);
	wprintf(L"Duplicate handle value: %d\n", (INT)hThisProcess);

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

	// 일시정지
	rewind(stdin);
	getchar();

	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
	CloseHandle(hThisProcess);
	return 1000;
}
