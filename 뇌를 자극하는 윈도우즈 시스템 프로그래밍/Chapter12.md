# Chapter12 쓰레드의 생성과 소멸
## Section01 Windows에서의 쓰레드 생성과 소멸
### 쓰레드의 생성
1. CreateThread
    <pre><code>
    기능: 쓰레드 생성
    HANDLE CreateThread(
        LPSECURITY_ATTRIBUTES   lpThreadAttributes, // 생성될 쓰레드의 보안 속성(+ 핸들 상속 여부)
        SIZE_T                  dwStackSize,        // 스택 크기, 0 전달 시 기본 1MB
        LPTHREAD_START_ROUTINE  lpStartAddress,     // 쓰레드 함수 시작 주소
        __drv_aliasesMem LPVOID lpParameter,        // 쓰레드 함수 전달 인자
        DWORD                   dwCreationFlags,    //         LPDWORD                 lpThreadId
    );
    </code><//pre>
