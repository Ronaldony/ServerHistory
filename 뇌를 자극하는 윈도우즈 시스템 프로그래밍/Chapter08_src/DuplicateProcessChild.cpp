#include <stdio.h>
#include <string.h>
#include <windows.h>

INT wmain(INT argc, WCHAR* argv[])
{
	HANDLE hParentProcess = (HANDLE)_wtoi(argv[1]);	// 전달받은 핸들
	DWORD exitCode;
	
	// 부모 프로세스 종료 대기
	WaitForSingleObject(hParentProcess, INFINITE);
	// 종료코드 저장
	if (GetExitCodeProcess(hParentProcess, &exitCode) == false)
	{
		wprintf(L"GetExitCodeProcess fail\n");
		return -1;
	}
	// 결과 출력
	wprintf(L"hParentProcess: %d, Exit code: %d\n", (INT)hParentProcess, exitCode);

	// 일시정지
	rewind(stdin);
	getchar();

	return 0;
}
