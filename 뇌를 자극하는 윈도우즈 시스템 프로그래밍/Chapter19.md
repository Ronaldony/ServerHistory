# Chapter19 비동기 I/O와 APC
## Section01 비동기 I/O
### 비동기 I/O의 이해
1. 동기와 비동기 I/O
    1) 동기: I/O와 CPU연산이 독립적으로 차례차례 실행되는 방식이다.
        * 동기 I/O는 블로킹 함수를 사용한다.
    2) 비동기: I/O와 CPU연산이 병행되어 실행되는 방식이다.
        * 비동기 I/O는 넌블로킹 함수를 사용한다.
    * <img width=600 src="https://user-images.githubusercontent.com/95362065/150155790-d23b5084-8b9b-496f-befc-67df887fcca9.png">
2. 동기 I/O의 단점
    1) I/O 작업을 하는 동안 CPU가 하는 일이 없게 된다.
3. I/O라고 하면 네트워크 I/O, 파일 I/O, 모니터 I/O 등 여러가지 종류가 있다.

### 중첩 I/O
1. Windows에서 제공하는 비동기 I/O 방식 중에서 **가장 대표적인 것**이 중첩 I/O이다.
2. **넌블로킹 함수**는 작업의 완료에 상관없이 바로 반환해 버리는 특성을 지닌다.
3. Windows 중첩 I/O 특성
    * <img width=600 src="https://user-images.githubusercontent.com/95362065/150247377-121e30e3-20f4-4d28-80d8-0d21555fd60f.png">
    * 위 그림과 같이 중첩 I/O는 넌 블로킹 I/O 함수를 중첩으로 호출 할 수 있는 특성을 갖는다.
4. 비동기 I/O는 파일보다 네트워크 통신에서 더 큰 의미를 갖는다.
    * 이유: 

### 중첩 I/O 예제
1. 본 책에서는 파이프 통신을 이용한 중첩 I/O 예제를 제시한다.
2. 중첩 I/O 기반 파이프 통신
    1) 1단계: 구조
        * <img width=650 src="https://user-images.githubusercontent.com/95362065/150267391-e5519d86-ace8-4ad2-8369-ef7a59d8b2ff.png">
    2) 2단계: OVERLAPPED 구조체 변수 선언
        <pre><code>
        typedef struct _OVERLAPPED {
            ULONG_PTR Internal;         // Windows 시스템이 사용하기 위한 멤버, 무조건 0으로 초기화
            ULONG_PTR InternalHigh;     // 위 멤버와 동일
            union {
                struct {
                    DWORD Offset;       // 파일 포인터 위치
                    DWORD OffsetHigh;   // 
                } DUMMYSTRUCTNAME;
                PVOID Pointer;          
            } DUMMYUNIONNAME;
            HANDLE    hEvent;           // 이벤트 오브젝트 핸들, 입출력 연산이 완료되었음을 확인하기 위한 오브젝트 
        } OVERLAPPED, *LPOVERLAPPED;
        </code></pre>
    3) 3단계: 파이프 생성 및 OVERLAPPED 구조체 변수 초기화가 끝나면, WriteFile 함수를 호출 시 FILE_FLAG_OVERLAPPED가 설정된 파이프와 OVERLAPPED 구조체를 인자로 전달한다. 그래야 비로소 WriteFile 함수가 중첩 I/O 방식으로 동작하게 된다.
3. 예제 코드
    * .../Chapter19_src 폴더 참조
4. 주의할 점
    1) 중첩 I/O에서는 I/O 연산 자체에 병목현상이 일어날 수 있다. 이런 상황에서는 진행중이던 I/O 연산이 완료되었는지 확인한 후 다음 I/O 연산이 이루어져야 한다.
        * GetOverlappedResult 함수를 사용하여 이 상황을 해결할 수 있다.
    2) 동기 I/O 연산에서 최종 목적지인 클라이언트에게 데이터 전송이 완료되어야만 반환하는 구조가 아니라, 전송을 위해 할당된 내부 메모리 버퍼에 복사가 이뤄지고 나면 반환하는 구조이다.
        * 따라서, WriteFile 함수 호출 시 전송할 데이터 크기보다 파이프 통신 출력 버퍼를 작게 하여야 한다.
        * 실질적인 외부로의 데이터 전송을 위해 내부 버퍼를 만들어두고 그 버퍼에서 다시 가져다가 외부로 전송을 한다. 즉, 우리가 쓴다는 것은 그 버퍼에 쓰는 행위이다.

### 완료루틴(Completion Routine) 기반 확장 I/O
1. 완료루틴: I/O 연산이 완료되었을 때 실행되는 루틴(함수)
2. 완료루틴 확장 I/O 특징
    1) 완료루틴 확장 I/O는 중첩 I/O 기능 + 완료루틴 호출 기능을 기본으로 한다.
        * 때문에 위 중첩 I/O에서 사용된 이벤트 오브젝트가 필요 없다.
    2) WriteFile / ReadFile 함수 대신 WriteFileEx / ReadFileEx 함수를 호출한다.
        * WriteFileEx / ReadFileEx
            <pre><code>
            BOOL WriteFileEx(
                HANDLE                          hFile,                  // lpCompletionRoutine 제외 WriteFile과 동일
                LPCVOID                         lpBuffer,
                DWORD                           nNumberOfBytesToWrite,
                LPOVERLAPPED                    lpOverlapped,            
                LPOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine     // 출력 작업이 끝난 후 호출될 완료루틴
            );

            BOOL ReadFileEx(
                HANDLE                          hFile,                  // lpCompletionRoutine 제외 ReadFile과 동일
                LPVOID                          lpBuffer,
                DWORD                           nNumberOfBytesToRead,
                LPOVERLAPPED                    lpOverlapped,
                LPOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine     // 입력 작업이 끝난 후 호출될 완료루틴
            );
            
            // typedef LPOVERLAPPED_COMPLETION_ROUTINE 
            typedef
            VOID
            (WINAPI *LPOVERLAPPED_COMPLETION_ROUTINE)(
                _In_    DWORD dwErrorCode,
                _In_    DWORD dwNumberOfBytesTransfered,
                _Inout_ LPOVERLAPPED lpOverlapped
            );
            </code></pre>
    
### 알림 가능한 상태(Alertable State)
1. Windows는 완료루틴 확장 I/O 연산이 완료되어 완료루틴을 실행되는 시점을 개발자가 정하도록 하였다. 그 시점은 SleepEx 함수를 통해 정할 수 있다.
2. SleepEx
    <pre><code>
    DWORD SleepEx(
        DWORD dwMilliseconds,   // 쓰레드 실행 정지 시간
        BOOL  bAlertable        // true: 쓰레드를 알림 가능한 상태로 변경, false: 일반 Sleep 함수와 동일하게 동작
    );
    </code></pre>
    * 쓰레드가 "알림 가능한 상태"가 되면 완료루틴의 호출이 이뤄진다. 호출되어야 할 완료루틴이 둘 이상인 경우에는 모두 다 호출된다.(SleepEx를 중복 호출할 필요가 없음)
    * 만약, bAlertable가 true이고, dwMilliseconds 시간 초과 시 I/O 작업이 완료되지 않으면 **완료루틴을 호출하지 않는다**.
    * 중첩 I/O를 해야하는 경우 모든 I/O 연산을 실행하고 마지막에 쓰레드를 "알림 가능한 상태"로 변경해야 한다.
3. SleepEx 이 외 "알림 가능한 상태"로 만드는 함수들
    * WaitForSingleObjectEx, WaitForMultipleObjectsEx
4. 비동기 I/O의 문제
    1) 비동기 I/O에서 커널 오브젝트에 존재하는 파일의 위치 정보는 의미가 없다.
        * 예를 들어 하나의 파일에 연속적으로 WriteFile 비동기 I/O 연산을 하는 경우 파일 I/O가 정상적으로 이루어지지 않는다.
        * 해결 방법: WriteFile에 인자로 들어가는 OVERLAPPED의 Offset, OffsetHigh 멤버를 활용하여 위 문제를 해결할 수 있다.
            <pre><code>
            // hFile: 파일 핸들
            // dataBufx: 전송할 데이터 버퍼
            // bytesWritex: 전송 데이터 크기
            // overlappedInst: OVERLAPPED 구조체
            // FileIOCompletionRoutine: 완료 루틴 주소

            OVERLAPPED overlappedInstPtr;
            overlappedInstPtr.Offset = 0;

            OVERLAPPED overlappedInstOne;
            overlappedInstOne.Offset = 0;
            WriteFileEx(hFile, dataBuf1, bytesWrite1, &overlappedInstOne, FileIOCompletionRoutine);
            overlappedInstPtr.Offset += wcslen(dataBuf1) * sizeof(wchar_t);     // 파일의 위치 정보 갱신

            OVERLAPPED overlappedInstTwo;
            overlappedInstTwo.Offset = overlappedInstPtr.Offset;                // 파일의 위치 정보 초기화
            WriteFileEx(hFile, dataBuf2, bytesWrite2, &overlappedInstTwo, FileIOCompletionRoutine);
            overlappedInstPtr.Offset += wcslen(dataBuf2) * sizeof(wchar_t);        // 파일의 위치 정보 갱신        
            </code></pre>

### 타이머에서의 완료루틴
1. SetWaitableTimer 함수의 완료루틴
    <pre><code>    
    BOOL SetWaitableTimer(
        HANDLE              hTimer,                     // 타이머 핸들
        const LARGE_INTEGER *lpDueTime,                 // 타이머 시간
        LONG                lPeriod,                    // 주기적 타이머
        PTIMERAPCROUTINE    pfnCompletionRoutine,       // 완료루틴 지정(아래 추가 정보)
        LPVOID              lpArgToCompletionRoutine,   // 완료루틴의 첫 번째 전달인자 정보
        BOOL                fResume
    );
    
    // 완료루틴 인자 함수
    typedef
    VOID
    (APIENTRY *PTIMERAPCROUTINE)(
        _In_opt_ LPVOID lpArgToCompletionRoutine,   // SetWaitableTimer 함수의 lpArgToCompletionRoutine 데이터
        _In_     DWORD dwTimerLowValue,             // 타이머가  Signaled 상태가 된 시간 정보
        _In_     DWORD dwTimerHighValue             // 타이머가  Signaled 상태가 된 시간 정보
    );    
    </code></pre>
    
### 지금까지의 내용 정리
1. Windows는 기본적으로 두 가지 방식의 비동기 I/O를 지원한다. 중첩 그리고 완료루틴 확장 I/O 방식이다.
2. 비동기 I/O는 넌블로킹 함수, 동기 I/O는 블로킹 함수를 사용한다.
3. 블로킹 함수 I/O는 함수를 반환하는 시점과 I/O 연산 완료 시점이 일치한다. 반면 넌블로킹 함수는 I/O 연산의 완료와 상관없이 함수를 반환한다.

## Section02 APC(Asynchronous Procedure Call)
### APC
1. APC: 비동기 함수 호출 매커니즘을 의미한다.
2. 앞서 소개한 ReadFileEx, WriteFileEx, SetWaitableTimer 등의 함수는 APC를 기반으로 구현된 함수들이다.

### APC의 구조
1. APC는 크게 "User-mode APC", "Kernel-mode APC" 이 두 가지 종류로 나뉜다. Kernel-mode PAC는 다시 "Normal"과 "Special"로 나뉜다.
2. **모든 쓰레드**는 자신만의 APC Queue를 가지고 있다. 
    * <img width=650 src="https://user-images.githubusercontent.com/95362065/150547377-5f31bd31-f467-4969-952b-9e72becefbbd.png">
    * 위 그림처럼 APC Queue에는 비동기적으로 호출되어야 할 함수들과 매개변수 정보가 저장된다. 그러나 쓰레드가 알림 가능 상태가 되어야만 비로소 호출된다.

### APC Queue의 접근
1. APC에 호출 함수를 등록하는 함수
    <pre><code>
    DWORD QueueUserAPC(
        PAPCFUNC pfnAPC,    // APC에 등록할 함수(완료루틴)
        HANDLE hTrhead,     // APC Queue 소유 쓰레드 핸들
        ULONG_PTR dwData    // 함수에 전달될 인자
    );
    </code></pre>
