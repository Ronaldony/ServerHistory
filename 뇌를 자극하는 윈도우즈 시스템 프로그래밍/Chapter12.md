# Chapter12 쓰레드의 생성과 소멸
## Section01 Windows에서의 쓰레드 생성과 소멸
### 쓰레드의 생성
1. CreateThread
    <pre><code>
    기능: 쓰레드 생성
    반환: 쓰레드 핸들
    HANDLE CreateThread(
        LPSECURITY_ATTRIBUTES   lpThreadAttributes, // 생성될 쓰레드의 보안 속성(+ 핸들 상속 여부)
        SIZE_T                  dwStackSize,        // 스택 크기, 0 전달 시 기본 1MB
        LPTHREAD_START_ROUTINE  lpStartAddress,     // 쓰레드 함수 시작 주소
        __drv_aliasesMem LPVOID lpParameter,        // 쓰레드 함수 전달 인자
        DWORD                   dwCreationFlags,    // 쓰레드의 생성 및 실행 조절 옵션. 쓰레드 초기 상태 blocked, 초기 스택 reserve 크기 지정 등이 있다. 
        LPDWORD                 lpThreadId          // 쓰레드 ID
    );
    </code></pre>
    * 참조: https://docs.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-createthread
2. 쓰레드는 메모리가 허용되는 만큼 생성될 수 있다. 즉, 쓰레드들의 독립적인 스택을 할당할 수 있을때까지 쓰레드를 생성을 허용한다.
3. main 쓰레드의 종료는 프로세스의 종료로 이어진다. 따라서 작업이 완료되지 않은 쓰레드라 하여도 종료된다.
4. 멀티 쓰레드 기반 프로그램은 흐름의 예측이 불가능하다.

### 쓰레드의 소멸
1. 쓰레드의 종료
    1) return 문을 이용한 쓰레드 종료
    2) ExitThread 함수 호출 쓰레드 종료
        <pre><code>
        기능: 해당 함수를 호출한 쓰레드가 강제 종료된다.
        void ExitThread(
            DWORD dwExitCode    // 커널 오브젝트에 저장할 종료 코드
        );
        </code></pre>
        * 참조: https://docs.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-exitthread
    3) TerminateThread 함수 호출 쓰레드 종료
        <pre><code>
        기능: hThread로 지정한 쓰레드가 강제 종료된다.
        BOOL TerminateThread(
            HANDLE hThread,     // 종료할 쓰레드 핸들
            DWORD  dwExitCode   // 커널 오브젝트에 저장할 종료 코드
        );
        </code></pre>
        * 참조: https://docs.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-terminatethread
2. 쓰데르 종료 코드
    1) GetExitCodeThread
        <pre><code>
        기능: hThread의 종료 코드를 얻는다.
        BOOL GetExitCodeThread(
            HANDLE  hThread,        // 대상 쓰레드
            LPDWORD lpExitCode      // 종료될 쓰레드의 종료 코드
        );
        </code></pre>
        * 참조: https://docs.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-getexitcodethread
3. 쓰레드 종료는 return 문을 이용한 종료가 **가장 이상적**이다.
     * 그 이유로는 TerminateThread, ExitThread 함수 사용 시 객체의 소멸자 호출이 안되어 메모리 누수가 발생할 수 있다.

## Section02 쓰레드의 성격과 특성
### 동시접근에 있어서의 문제점
1. 두 쓰레드가 같은 데이터를 사용중인 경우 발생할 수 있는 문제점
    1) A 쓰레드가 데이터에 먼저 접근하여 레지스터에 저장 후 연산
    2) A 쓰레드가 결과를 메모리에 적재하지 않은 상태에서 B 쓰레드가 CPU 점유(컨텍스트 스위칭)
    3) B 쓰레드가 데이터에 접근하여 연산 및 메모리 적재 후 A 쓰레드에게 CPU 양도
    4) A 쓰레드가 결과를 메모리에 적재
    * 결과적으로 B 쓰레드의 결과가 A 쓰레드의 결과로 덮어씌어졌다.

### 프로세스로부터의 쓰레드 분리
1. 핸들 테이블은 프로세스에 종속적이며, 같은 프로세스 내 쓰레드들은 이를 공유한다.
2. 프로세스로부터의 쓰레드 분리
    * 자식 프로세스의 경우 부모 프로세스로부터 생성되면 Usage Count가 2가 된다. 쓰레드도 마찬가지로 쓰레드가 생성된 시점에 Usage Count는 2가 된다. 이 때 CloseHandle을 호출하여 해당 쓰레드의 Usage Count를 1로 만드는 것이 **프로세스로부터의 쓰레드 분리**라고 한다.

### ANSI 표준 C 라이브러리와 쓰레드
1. 초기 표준 C 라이브러리 설계 시 쓰레드에 대한 고려가 이뤄지지 않았다. 때문에 멀티 쓰레드 기반 프로그램에서 표준 C 라이브러리 사용 간 동일 메모리 영역을 동시 접근하는 불상사가 발생했었다. (대표적으로 strtok 함수)마이크로소프트는 이러한 문제에 대하여 멀티 스레드에 안전한 표준 C 라이브러리를 제공한다.
    <img width=600 src="https://user-images.githubusercontent.com/95362065/148686025-3e948914-d5e7-4e21-86d1-391fbc14d2c7.png">
2. _beginthreadex
    1) 안전한 표준 C 라이브러리에서 제공하는 쓰레드 생성 함수이며, 쓰레드의 독립적인 메모리 블록을 할당시킨다.
    <pre><code>
    기능: 쓰레드를 생성하며, 인자는 CreateThread 함수와 동일한 인자 구성을 같는다.
    헤더: process.h
    uintptr_t _beginthreadex(
        void *security,
        unsigned stack_size,
        unsigned ( __stdcall *start_address )( void * ),
        void *arglist,
        unsigned initflag,
        unsigned *thrdaddr
    );
    </code></pre>
    2) _beginthreadex
    <pre><code>
    기능: 내부적으로 _beginthreadex에서 할당된 쓰레드 독립 메모리를 해제하고 ExitThread를 호출한다.
    void _endthreadex(
        unsigned retval
    );
    </code></pre>

## Section03 쓰레드의 상태 컨트롤
### Suspend & Resume
1. Suspend & Resume
    <pre><code>
    // 쓰레드를 Blocked 상태에 두는 함수
    DWORD SuspendThread(HANDLE hThread);
    // 쓰레드를 Ready 상태에 두는 함수
    DWORD ResumeThread(HANDLE hThread);
    </code></pre>
2. 서스펜드 카운트(Suspend Count): SuspendThread 호출 빈도수를 나타내며 쓰레드의 커널 오브젝트에 기록됨.
    * CreadThread로 쓰레드 생성 시에 Suspend 옵션을 지정하면 서프펜드 카운트 1로 생성된다.
    * ResumeThread 함수 호출 시 서스펜드 카운트 1 감소한다.

## Section04 쓰레드의 우선순위 컨트롤
### 기준 우선순위와 상대적 우선순위
1. 쓰레드의 우선순위는 프로세스의 기준 우선순위와 쓰레드의 상대적 우선순위의 **조합**으로 결정된다.
    * 참조: https://docs.microsoft.com/ko-kr/windows/win32/procthread/scheduling-priorities
2. 관련 함수
    <pre><code>
    // 쓰레드의 상대적 우선순위 변경
    BOOL SetThreadPriority(HANDLE hThread, int nPriority);
    // 쓰레드의 상대적 우선순위 값 반환
    int GetThreadPriority(HANDLE hThread);
    </code></pre>
