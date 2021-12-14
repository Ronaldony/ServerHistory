# Chapter05  프로세스의 생성과 소멸
## Section01 프로세스의 이해
### 프로세스란 무엇인가?
1. 프로세스란 메인 메모리 공간에 바이너리 코드가 올라간 상태의 프로그램을 의미한다. 즉 실행중인 프로그램

### 프로세스를 구성하는 요소
1. 프로세스의 메모리 공간은 Data, Stack, Heap, Code 영역으로 나뉘어진다.
2. Register Set도 프로세스의 일부로 포함시켜 말할 수 있다.

## Section02 프로세스의 스케줄링과 상태 변환
### 프로세스의 스케줄링
1. 현대 모든 운영체제는 멀티 프로세스 실행 기반이다. 때문에 CPU가 여러 개의 프로세스를 특정 순서대로 실행시킨다. 이것을 스케줄링이라 한다.
2. 스케줄링의 기본 원리
    1) 프로세스의 cpu 할당 순서 및 방법을 결정짓는 일
    2) 스케줄러에 의해 결정된다.
3. 일반적으로, 프로그램이 실행되는 과정에서 많은 시간을 I/O(데이터 입력 및 출력)에 할당한다.
    * 데이터 입출력: 파일 입출력, 네트워크 데이터 송수신, 등

### 프로세스의 상태 변화
1. 프로세스의 상태 변화
    * <img width="400" src="https://user-images.githubusercontent.com/95362065/145715593-e8667741-8f15-41f5-8988-8e590b9ba552.png">
    1) 상황 1: S(Start)->Ready 상태로 전이
        * 프로세스는 생성과 동시에 Ready 상태가 된다.
    2) 상황 2: Ready->Running 상태로 전이
        * 스케줄러는 Ready 상태의 프로세스 중 스케줄링 알고리즘을 이용해 선택하여 Running 상태로 전환한다.
    3) 상황 3: Running->Ready 상태로 전이
        * Running 상태의 프로세스의 우선순위가 새로 생성된 프로세스보다 낮거나(선점형) 할당된 타임 슬라이스가 만료되는 경우 Ready 상태로 전환한다.
    4) 상황 4: Running->Blocked 상태로 전이
        * Running 상태의 프로세스가 일반적으로 데이터 I/O 작업 시 Blocked 상태로 전환한다. 데이터 I/O 작업은 CPU를 점유하는 일이 아니기 때문이다.
    5) 상황 5: Blocked->Ready 상태로 전이
        * Blocked 상태의 프로세스가 데이터 I/O가 완료되면 Ready 상태로 전환한다. Ready 상태는 스케줄러에 의해 선택되어 실행 가능한 상태이고, Blocked 상태는 스케줄러에 의해 선택될 수 없는 상태이다.

## Section03 컨텍스트 스위칭
### 프로세스와 시스템 부하
1. 스케줄러에 의해 실행되는 프로세스가 전환될 경우 프로세스별로 관리되는 데이터 저장과 CPU 내 Register set가 전환되는 프로세스의 데이터로 채워지는 작업을 한다. 이를 컨텍스트 스위칭이라 부른다.
2. 컨텍스트 스위칭은 시스템에 많은 부담을 준다. 이는 멀티 프로세스 운영체제의 단점이 된다.

## Section04 프로세스의 생성
### CreateProcess 함수의 이해
1. CreateProcess 함수를 호출하면 자식 프로세스를 생성하게 된다. 이 때 CreateProcess 함수를 호출한 프로세스는 부모 프로세스가 된다.
2. CreateProcess 함수 선언 형태
    <pre><code>
    BOOL CreateProcessA(
    LPCSTR                lpApplicationName,      // 실행파일 이름
    LPSTR                 lpCommandLine,          // 실행파일 명령행(표준 검색경로 기준 실행파일 탐색)
    LPSECURITY_ATTRIBUTES lpProcessAttributes,    // 프로세스 보안 속성
    LPSECURITY_ATTRIBUTES lpThreadAttributes,     // 스레드 보안 속성
    BOOL                  bInheritHandles,        // 핸들 테이블 상속 여부
    DWORD                 dwCreationFlags,        // 프로세스 특성
    LPVOID                lpEnvironment,          // 환경블럭 옵션
    LPCSTR                lpCurrentDirectory,     // 자식 프로세스의 현재 디렉터리 설정 
    LPSTARTUPINFOA        lpStartupInfo,          // STARTUP_INFO 구조체
    LPPROCESS_INFORMATION lpProcessInformation    // PROCESS_INFORMATION 구조체
    );
    * 출처: https://docs.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-createprocessa
    </code></pre>    
3. 프로세스 생성
    1) 1단계 STARTUPINFO 구조체 변수의 생성 및 초기화
    <pre><code>
    typedef struct _STARTUPINFOA {
      DWORD  cb;              // 구조체 변수의 크기(버전 관리)
      LPSTR  lpReserved;
      LPSTR  lpDesktop;
      LPSTR  lpTitle;         // 콘솔 윈도우의 타이틀 바 제목
      DWORD  dwX;             // 프로세스 윈도우의 X 좌표
      DWORD  dwY;             // Y 좌표
      DWORD  dwXSize;         // 가로 길이
      DWORD  dwYSize;         // 세로 길이
      DWORD  dwXCountChars;
      DWORD  dwYCountChars;
      DWORD  dwFillAttribute;
      DWORD  dwFlags;         // 설정된 멤버의 정보
      WORD   wShowWindow;
      WORD   cbReserved2;
      LPBYTE lpReserved2;
      HANDLE hStdInput;
      HANDLE hStdOutput;
      HANDLE hStdError;
    } STARTUPINFOA, *LPSTARTUPINFOA;
    * 출처: https://docs.microsoft.com/en-us/windows/win32/api/processthreadsapi/ns-processthreadsapi-startupinfoa
    </code></pre>
    2) 2 단계: 현재 디렉터리의 설정
        * 일반적으로 프로세스가 생성되면 프로세스의 현재 디렉터리는 프로세스의 실행파일이 존재하는 디렉터리로 설정한다.
        * GetCurrentDirectory, SetCurrentDirectory 함수
        <pre><code>
        DWORD GetCurrentDirectory(
          DWORD  nBufferLength,
          LPTSTR lpBuffer
        );
        BOOL SetCurrentDirectory(
          LPCTSTR lpPathName
        );
        * 출처: https://docs.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-getcurrentdirectory, 
        https://docs.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-setcurrentdirectory
        </code></pre>
    3) 3 단계: CreateProcess 함수 호출한다. 
    <pre><code>
    * 예시: calculator.exe라는 프로세스를 생성 시 (+명령행 인자에 1, 2입력)
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    // si, pi 초기화...
    bool isCreated = CreateProcess(
        NULL, "calculator.exe 1 2",
        NULL, NULL,
        TRUE,   // 핸들 테이블 상속
        CREATE_NEW_CONSOLE,
        NULL, NULL,
        &si,
        &pi);
    </code></pre>
    * CreateProcess 함수 2번째 인자에서 실행파일에 해당하는 "calculator.exe"는 **표준 검색경로** 순서대로 찾게 된다.
    * 표준 검색경로

        1. 실행파일이 존재하는 디렉터리
        2. 프로세스의 현재 디렉터리
        3. Windows 시스템 디렉터리
        4. Windows 디렉터리
        5. 환경변수 PATH에 의해 지정된 디렉터리

## Section05 프로세스 생성과 예제 그리고 문제점
* 생략

## Section06 명령 프롬프트 프로젝트 기능 추가
* 링크: (TODO: 추가할 것)
