# Chapter07 스레드 스케줄링, 우선순위, 그리고 선호도
## 들어가기에 앞서
### 컨텍스트 전환
1. 윈도우는 매 20ms마다 모든 스레드 커널 오브젝트 중 스케줄 가능 상태에 있는 스레드 커널을 선정하여 **컨텍스트 전환**한다.
    * 컨텍스트 구조체 내에 저장된 레지스터 값을 CPU 레지스터에 로드하는 것
2. SPY++ 도구를 이용하여 스레드의 컨텍스트 전환 회수, 우선순위, 스레드의 속성 정보 등을 모니터링 할 수 있다.
3. 시스템 내 대부분의 스레드들은 보통 스케줄이 불가능한 상태에 있다.
    1) 이유 1: 정지 카운트 > 0
    2) 이유 2: I/O 작업으로 인한 Blocked 상태
    3) 등..

## Section01 스레드의 정지와 계속 수행
### 정지 카운트
1. 스레드는 정지 카운트가 0일 때 I/O 작업등 이유로 Blocked 상태가 아닌 이상 **스케줄 가능한 상태(Ready)**가 된다.
2. SuspendThread 호출 시 세심한 주의가 필요하다. 만일 스레드가 힙 할당 중 SuspendThread를 호출하여 정지되면, 이 스레드는 힙을 잠그게 된다. (힙 내부에서 HeapLock) 이때 다른 스레드가 힙 할당 접근 시 정지된다.
    * 따라서 SuspendThread 함수는 스레드가 정지될 때 발생하는 문제와 데드락을 피할 수 있는 명확한 방법이 있는 경우에만 사용되어야 한다.
    * SuspendThread 함수 호출 시 해당 스레드가 커널 모드의 코드를 수행 시 커널 모드 작업이 끝나고 스레드가 정지된다.

## Section02 프로세스의 정지와 계속 수행
### 프로세스의 정지
1. Windows Sysinternals가 제공하는 프로세스 익스플로러의 "Suspend Process" 기능을 사용하면 프로세스 내 모든 스레드를 정지시키는 효과를 낼 수 있다.
2. 윈도우는 Race condition을 방지하기 위해 디버깅 메커니즘 외에는 프로세스 내 모든 스레드를 정지시키는 방법을 제공하지 않는다.

## Section03 슬리핑
### Sleep 함수
1. Sleep 함수의 기능
    1) 스레드에 남은 타임 슬라이스를 포기한다.
    2) 지정 시간동안 스케줄러가 해당 스레드를 스케줄링하지 않는다.
    3) 매개변수로 0 전달 시 남은 타임 슬라이스를 포기하고 스케줄 가능 상태가 된다. (선점형 OS이므로 해당 스레드보다 낮은 순위의 스레드는 이때도 스케줄링 대상에서 밀린다.)

## Section04 다른 스레드로의 전환
### 스케줄 전환
1. SwitchToThread 함수
    1) 함수 원형: BOOL SwitchToThread(void)
    2) 기능: SwitchToThread 함수 호출 시 시스템은 일정 시간동안 CPU 할당을 받지못한 스레드가 있는지 확인한다. 없는 경우 함수는 바로 반환되지만, 그러한 스레드가 존재하면 해당 스레드를 스케줄한다. 해당 스레드는 단일 퀀텀 시간동안만 수행되며, 이후 스케줄러는 이전과 동일하게 스케줄링한다.
        * 아마 이 스케줄되는 스레드는 SwitchToThread를 호출한 스레드보다 우선순위가 낮을 것이다.
        * Sleep(0)과 유사한듯 차이점이 있다. 이 점을 기억하자!

## Section05 하이퍼스레드 CPU 상에서 다른 스레드로의 전환
### 하이퍼스레딩
1. 하이퍼스레드 기반 프로세서 칩
    1) 구조: 다수의 "논리적" CPU를 가지며 각기 다른 스레드를 수행한다. 이 같은 칩에서 수행되는 스레드들은 자신만의 구조적 상태(레지스터 정보)를 가지지만, CPU 캐시와 같은 주요 수행 자원은 공유하게 된다.
    2) 동작: 특정 스레드가 일시적으로 멈추게 되면 운영체제의 개입 없이도자동적으로 다른 스레드를 수행한다. 스레드의 일시적 멈춤은 캐시 미스, 분기 예측 실패, 앞서 수행한 인스트럭션 결과에 대한 대기 등에 의해 발생한다.
    3) 기타: 스핀 루프를 수행하는 경우 다른 스레드가 공유 리소스에 접근할 수 있도록 현재 스레드를 일시적으로 멈추는 것이 좋다. x86의 경우 PAUSE 명령으로 메모리 순서 위반을 회피하여 성능 향상에 도움이 된다.
        * 잘 이해가 되지 않아 조사 필요
        * PAUSE 명령은 WinNT.h에 YieldProcessor 매크로를 사용하면 수행된다.
        * 스핀 루프(=스핀 락): 크리티컬 섹션에 도달했을 때 CPU 점유를 포기하는 것이 아닌 CPU륻 점유한 상태에서 뺑뺑이를 돌며 임계 영역 접근 권한을 기다리는 동작

## Section06 스레드 수행 시간
### 시간 구하기
1. 많은 프로그래머가 다음과 같은 코드로 스레드의 수행 시간을 구한다.
    <pre><code>
    ULONGLONG start = GetTickCount64();
    // 알고리즘
    ULONGLONG End = GetTickCount64() - start;
    </code></pre>
    1) 문제점: 위 코드의 수행 중 인터럽트가 수행되지 않는다는 가정이 필요하다. 선점형 OS이므로 언제든지 다른 스레드에 의해 CPU 할당권이 넘어갈 수 있음
    2) 해결 방법: 실제 스레드가 부여받은 CPU 시간이 얼마나 되는지 알아내는 함수를 이용하는 것!
        * GetThreadTimes 함수를 이용하여 해당 문제를 해결할 수 있다.

### GetThreadTimes
1. 함수 원형
    <pre><code>
    // 실제 스레드가 부여받은 CPU 시간
    // 반환되는 모든 시간은 100ns 단위의 시간이다.
    BOOL GetThreadTimes(
        HANDLE     hThread,         // 시간 값을 얻을 스레드 핸들
        LPFILETIME lpCreationTime,  // 생성 시간. 1601/1/1 자정으로부터 스레드가 생성된 시점까지의 시간
        LPFILETIME lpExitTime,      // 종료 시간. 1601/1/1 자정으로부터 스레드가 종료된 시점까지의 시간
        LPFILETIME lpKernelTime,    // 커널 시간. 커널 모드에서 OS가 제공하는 코드를 수행하는 데 소요된 CPU 시간
        LPFILETIME lpUserTime       // 유저 시간. 응용 코드를 수행하는 데 소요된 CPU 시간
    );
    </code></pre>
2. 응용
    <pre><code>
    GetThreadTimes(..., lpKernelTimeStart, lpUserTimeStart);
    // 알고리즘
    GetThreadTimes(..., lpKernelTimeEnd, lpUserTimeEnd);
    
    // 알고리즘이 총 소요한 시간 = 커널 모드 시간 + 유저 모드 시간
    total = (lpKernelTimeEnd - lpKernelTimeStart) + (lpUserTimeEnd - lpUserTimeStart);
    </code></pre>

### CPU 시간 계산
1. 윈도우 비스타 이전의 OS는 클록 타이머(ClockRes 사용)를 기반으로 10-15ms 단위로 CPU 시간을 계산하였다. 비스타부터는 OS 시작 이후 시간을 저장하는 TSC를 사용하여 CPU 시간을 계산한다.
2. 스레드가 스케줄러에 의해 정지되면 현재의 TSC 값과 스레드가 재시작되었던 시점에 획득된 TSC 값과의 차를 계산한 후 수행 시간에 더해준다.

## Section07 컨텍스트 내의 CONTEXT 구조체
### CONTEXT 구조체
1. 역할: 시스템이 저장하는 스레드의 상태 정보로, 다음번에 CPU가 스레드를 수행할 때 어디서부터 수행을 시작해야 할지를 알려주는 역할
2. 구조: 프로세스별 레지스터 데이터를 가지고 있다.
    * MSDN에서는 CONTEXT 구조체 각 멤버들이 하는 역할을 설명하지 않고 있다. 그 이유는 CPU별로 서로 다르기 때문이다.
4. 관련 함수
    <pre><code>
    // 해당 스레드의 유저 모드의 CONTEXT 구조체 얻어오기
    BOOL GetThreadContext(
        HANDLE    hThread,
        LPCONTEXT lpContext // Get할 CONTEXT 구조체 주소
    );
    * GetThreadContext 함수를 호출하기 위해서는 반드시 SuspendThread를 호출하여 스레드를 정지한 상태여야 한다. SuspendThread 호출 시
    스레드가 커널 모드로 동작하는 경우에도 유저 CONTEXT 구조체 내용이 변경되지 않으므로 GetThreadContext는 정상적인 레지스터를 읽어온다.
    
    // 해당 스레드의 유저 모드의 CONTEXT 구조체 변경
    BOOL SetThreadContext(
        HANDLE        hThread,
        const CONTEXT *lpContext    // 
    );
    * GetThreadContext 함수와 마찬가지로 호출 전 SuspendThread를 호출해야 한다.
    </code></pre>
    
## Section08 스레드 우선순위
### 우선순위
1. 제로 페이지 스레드: 시스템 부팅 시 생성되는 특별한 스레드이다. 이 스레드는 0번 우선순위를 가진 유일한 스레드이며, 시스템 내 어떠한 스레드도 스케줄 가능 상태가 아닐 때 램의 사용되지 않는 페이지를 0으로 만들어주는 작업을 수행한다.

## Section09 우선순위의 추상적인 의미
### 호환성을 위한 MS의 선택 
1. MS가 시스템의 동작 방식을 변경하면서도 이전에 개발된 소프트웨어가 정상적으로 수행되도록 보장할 수 있었던 방법
    1) 스케줄러의 동작 방식을 완벽하게 문서화하지 않았다.
    2) 애플리케이션이 스케줄러의 기능상의 장점을 완벽하게 이용하지 못하도록 하였다.
    3) 스케줄러의 알고리즘은 변경될 수 있으므로 코드를 방어적으로 작성할 것을 지속적으로 알려왔다.
2. 최종 사용자가 우리가 개발한 어플 외에도 다른 어플을 동시에 사용할 가능성이 있는지에 대해 고려해야 한다. 그 상황에서 어플의 스레드가 어느 정도의 응답성이 요구되는지 판단하고, 이를 기준으로 프로세스의 우선순위 클래스를 결정해야 한다. 
3. 실시간 우선순위 클래스는 가능한 한 사용하지 않는 것이 좋다. OS가 운용하는 스레드들 조차도 대부분 이보다 낮아 디스크 I/O, 네트워크 트래픽, 키보드 및 마우스 입력 등의 동작을 방해할 수 가 있다.
    * 스케줄링 우선순위 향상 권한이 없는 사용자는 실시간 우선순위 클래스로 프로세스를 수행하지 못한다. 보통 관리자나 파워 유저가 이 권한을 가진다.

### 우선순위
1. 프로세스에는 프로세스 우선순위 클래스가 할당되고, 스레드에는 프로세스 우선순위에 상대적인 스레드 우선순위가 할당된다.
2. 프로세스 우선순위를 변경하는 방법
    1) SetPriorityClass 함수
    2) 명령 쉘: START /LOW CA LC.EXE
    3) 작업관리자

### I/O 요청 우선순위 스케줄링
1. 윈도우 비스타부터 스레드가 I/O 요청에 대해 우선순위를 지정할 수 있게 하였다.
    1) 방법
        <pre><code>
        // 각 스레드는 자신의 I/O 우선순위만 변경할 수 있다.
        SetThreadPriority(GetCurrentThread(), THREAD_MODE_BACKGROUND_BEGIN);    // 낮은 우선순위 I/O 요청
        SetThreadPriority(GetCurrentThread(), THREAD_MODE_BACKGROUND_END);      // 보통 우선순위 I/O 요청     
        
        // 프로세스 내 모든 스레드에 대한 IO 우선순위 변경
        SetPriorityClass(GetCurrentProcess(),  PROCESS_MODE_BACKGROUND_BEGIN);    // 낮은 우선순위 I/O 요청
        SetPriorityClass(GetCurrentProcess(), PROCESS_MODE_BACKGROUND_END);      // 보통 우선순위 I/O 요청     
        
        // 파일 단위로 지정하기
        SetFileInformationByHandle(...);
        </code></pre>
2. 우선순위 역전 현상
   1) 낮은 우선순위의 스레드가 요청한 I/O 작업으로 인해 보통 우선순위 스레드가 요청한 I/O 작업이 지연되는 현상

### 스케줄링 실습 예제 어플리케이션
