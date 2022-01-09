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
### 힙, 데이터 영역, 그리고 코드 영역의 공유에 대한 검증
