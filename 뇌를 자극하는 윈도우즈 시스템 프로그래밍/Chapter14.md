# Chapter14 쓰레드 동기화 기법2
## Section01 실행순서에 있어서의 동기화
### 실행순서의 동기화
1. 실행순서를 동기화한다 = 메모리에 접근하는 쓰레드의 실행순서를 동기화한다.

### 이벤트(Event) 기반 동기화
1. 관련 함수
    <pre><code>
    // 이벤트 오브젝트 생성
    HANDLE CreateEvent( 
        LPSECURITY_ATTRIBUTES lpEventAttributes,    // 보안 속성
        BOOL bManualReset,      // *이벤트 오브젝트의 수동/자동 리셋 모드 지정(아래에서 추가 설명)
        BOOL bInitialState,     // 이벤트 오브젝트의 초기 상태를 결정한다. true: signaled, false: non-signaled
        LPTSTR lpName           // 이벤트 오브젝트 이름
    );
    
    // Non-Signaled 상태로 전환
    BOOL ResetEvent(HANDLE hEvent);
    // Signaled 상태로 전환
    BOOL SetEvent(HANDLE hEvent);
    </code></pre>
2. 이벤트 오브젝트의 수동/자동 리셋 모드
    * 일반적인 커널 오브젝트는 초기 non-signaled 상태로 생성되었다가 사용 종료 시 signaled 상태로 **자동 변경**된다. 그러나 이벤트 오브젝트는 자동으로 signaled 상태가 되지 않는다.
    * 수동 리셋 모드: singlaed 상태가 되어 WaitForSingleObject 함수를 빠져 나와도 non-signaled 상태로 **자동 변경 되지 않는다**.
    * 자동 리셋 모드: 위와 같은 상황일 때 non-signaled 상태로 **자동 변경**된다.
3. 이벤트 오브젝트의 사용 예시
    1) 수동 리셋 모드
    <pre><code>
    // 이미 이벤트 오브젝트가 생성되고 Signaled인 상태
    WaitForSingleObject(hEvent);
    ResetEvent(hEvent);
    // 임계 영역
    SetEvnet(hEvent);
    </code></pre>
    * 수동 리셋 모드는 둘 이상의 쓰레드를 동시에 깨워서 실행해야 하는 경우 아주 좋은 도구이다.
    2) 자동 리셋 모드
    <pre><code>
    // 이미 이벤트 오브젝트가 생성되고 Signaled인 상태
    WaitForSingleObject(hEvent);
    // 임계 영역
    SetEvnet(hEvent);
    </code></pre>
    
## Section02 이벤트 더하기 뮤텍스
1. 
