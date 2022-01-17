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
        BOOL bManualReset,      // *이벤트 오브젝트의 수동(TRUE)/자동(FALSE) 리셋 모드 지정(아래에서 추가 설명)
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
* 생략

## Section03 타이머 기반 동기화
### 타이머 오브젝트
1. 타이머 오브젝트는 정해진 시간이 지나면 **자동으로 Signaled 상태**가 된다.
2. 종류
    1) 수동 리셋 타이머: 정해진 시간에만 Signaled 상태가 된다.
    2) 자동 리셋 타이머: 정해진 시간 이후 주기적으로 Signaled 상태로 전환한다.
3. 타이머 오브젝트는 생성 시 non-signaled 상태이다.

### 수동/자동 리셋 타이머
1. 관련 함수
    <pre><code>
    // 타이머 생성
    HANDLE CreateWaitableTimer(
        LPSECURITY_ATTRIBUTES lpTimerAttributes,    // 보안 속성
        BOOL                  bManualReset,         // 리셋 모드 지정 인자, TRUE: 수동, FALSE: 자동
        LPCWSTR               lpTimerName           // 타이머 이름
    );
    
    // 알람 시간 설정
    BOOL SetWaitableTimer(
        HANDLE              hTimer,                     // 타이머 오브젝트 핸들
        const LARGE_INTEGER *lpDueTime,                 // 알람이 울리는 시간(+는 절대 시간, -는 상대적 시간을 의미한다.), 100nsec 단위이다.
        LONG                lPeriod,                    // 주기적 알람 시간 설정 인자, 1msec 단위이다.
        PTIMERAPCROUTINE    pfnCompletionRoutine,       // 완료 루틴 타이머 설정 인자(19 장에서 소개)
        LPVOID              lpArgToCompletionRoutine,   // 위와 동일
        BOOL                fResume                     // 전원 관리 관련 인자(기본 FALSE)
    );
    </code></pre>
