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
