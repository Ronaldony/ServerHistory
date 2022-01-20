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
    
