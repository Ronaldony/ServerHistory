# Chapter13 쓰레드 동기화 기법 1
## Section01 쓰레드 동기화란 무엇인가?
### 두 가지 관점에서의 쓰레드 동기화
1. 실행 순서의 동기화
    * 쓰레드의 실행 순서를 정의하고, 이 순서에 반드시 따르도록 하는 동기화
2. 메모리 접근에 대한 동기화
    * 동일 메모리 영역에 동시접근을 막는 동기화, 대표적으로 데이터와 힙 영역이 있다.
3. 두 동기화의 차이점
    * 실행 순서의 동기화는 **접근의 순서**가 반드시 지켜져야 한다.
    * 메모리 접근의 동기화는 접근 순서와 상관없이 **동시접근**을 막아야 한다.

### 쓰레드 동기화에 있어서의 두 가지 방법
1. Windows에서 제공하는 동기화 기법은 아래 2가지로 나뉜다.
    1) 유저 모드 동기화
        * 동기화 과정 중 커널의 힘을 빌리지 않는 동기화 기법이다. 성능상 이점이 있지만 기능상 제한이 있다.
    2) 커널 모드 동기화
        * 커널에서 제공하는 동기화 기능을 사용한다. 다만 동기화 관련 함수 사용 시 커널 모드로의 전환이 일어나 성능 저하로 이어진다.

## Section02 임계 영역(Critical Section) 접근 동기화
### 임계 영역에 대한 이해
1. 임계 영역: 메모리 접근 혹은 실행 순서 동기화에 문제가 발생할 수 있는 코드 블럭 영역.
2. 동기화의 종류
    1) 유저 모드
        |종류|사용처|
        |---|---|
        |크리티컬 섹션|메모리 접근 동기화|
        |인터락 함수|메모리 접근 동기화|
    2) 커널 모드
        |종류|사용처|
        |---|---|
        |뮤텍스|메모리 접근 동기화|
        |세마포어|메모리 접근 동기화|
        |이름있는 뮤텍스|프로세스간 동기화|
        |이벤트|싱행순서 동기화|
    * 각 동기화 기능들은 용도가 정해져있지 않다. 적재적소에 맞게 사용되어야 한다.

##Section03 유저 모드의 동기화
### 크리티컬 섹션 기반 동기화
1. 크리티컬 섹션 오브젝트를 얻은 쓰레드가 실행되는 동기화 기법이다. 임계 영역에 대해 동기화가 필요한 경우 유용하다.
2. 관련 함수
    <pre><code>
    CRITICAL_SECTION gCriticalSection;  // 크리티컬 섹션 오브젝트
    
    // 크리티컬 섹션 오브젝트 초기화
    void InitialzeCriticalSection(LPCRITICAL_SECTION lpCriticalSection);
    
    // 크리티컬 섹션 오브젝트 제거
    void DeleteCriticalSection(LPCRITICAL_SECTION lpCriticalSection);
    
    // 크리티컬 섹션 오브젝트 획득
    // 주의: 이 함수를 호출 시에 다른 쓰레드에 의해 오브젝트가 사용중이라면 블로킹된다.
    void EnterCriticalSection(LPCRITICAL_SECTION lpCriticalSection);
    
    // 크리티컬 섹션 오브젝트 반환
    void LeaveCriticalSection(LPCRITICAL_SECTION lpCriticalSection);
    </code></pre>
    
### 인터락 함수 기반의 동기화
1. 인터락 함수를 호출하여 동기화하는 기법이다. 변수 하나의 접근에 대해서 동기화가 필요한 경우 유용하다.
2. 관련 함수
    <pre><code>
    // 인터락 함수는 내부적으로 한 순간에 하나의 쓰레드에 의해서만 실행되도록 동기화되어 있다.
    
    // 인자로 입력된 변수의 값을 1 증가시킨다.
    LONG InterlockedIncrement(LONG volatile* Addend);
    
    // 인자로 입력된 변수의 값을 1 감소시킨다.
    LONG InterlockedDecrement(LONG volatile* Addend);    
    </code></pre>
3. MS는 위에서 소개한 함수말고도 다양한 형태의 인터락 함수를 제공한다.

## Section04 커널 모드 동기화
### 뮤텍스 기반의 동기화
1. 뮤텍스 오브젝트(이하 뮤텍스)를 생성하여 동기화하는 방법이다.
2. 관련 함수
    <pre><code>
    // 뮤텍스 생성
    HANDLE CreateMutex(
        LPSECURITY_ATTRIBUTES lpMutexAttributes,    // 보안 속성(상속 관련)
        BOOL bInitialOwner,                         // 초기 뮤텍스 사용 여부
        LPCTSTR lpName                              // 뮤텍스 이름(이름있는 뮤텍스 생성 시 사용, 기본 값은 NULL)
    );
    
    // 뮤텍스 반환, 
    BOOL ReleaseMutex( HANDLE hMutex );    
    
    // 뮤텍스 획득
    WaitForSingleObject 함수 호출을 통해 뮤텍스를 획득한다.
    WaitForSingleObject에 전달된 인자를 Non-Signaled 상태로 변경시키는 특징을 이용한 것이다.
    </code></pre>
3. 뮤텍스 오브젝트는 획득이 가능한 경우 **Signaled** 상태가 되고, 사용 중인 경우 **Non-Signaled** 상태가 된다.
4. 뮤텍스 사용 절차
    <pre><code>
    CreateMutex(...);               // 뮤텍스 오브젝트 생성
    WaitForSingleObject(hMutex);    // 뮤텍스 획득
    // 임계 영역
    ReleaseMutex(hMutex);           // 뮤텍스 반환
    CloseHandle(hMutex);            // 뮤텍스 제거
    </code></pre>

### 세마포어 기반의 동기화
1. 뮤텍스와 유사한 동기화 기법이다. 보통 "뮤텍스는 세마포어의 일종이다"라고 말한다.
    * 세마포어는 뮤텍스와 달리 **카운트** 기능이 있다. 즉 임계 영역에 접근 가능한 쓰레드 개수를 조절하는 기능을 제공한다.
2. 관련 함수
    <pre><code>
    // 세마포어 생성
    HANDLE CreateSemaphore(
        LPSECURITY_ATTRIBUTES lpSemaphoreAttrubutes,    // 보안 속성
        LONG lInitialCount,     // 임계 영역 접근 쓰레드 초기 카운트
        LONG lMaximumCount,     // 임계 영역 접근 쓰레드 최대 개수 지정
        LPCTSTR lpName          // 세마포어 이름
    );
    
    // 세마포어 반환
    BOOL ReleaseSemaphore(
        HANDLE hSemaphore,      // 반환하고자하는 세마포어 핸들
        LONG lReleaseCount,     // 세마포어 카운트 증가 개수(최대 개수 초과시 함수가 false를 리턴한다.)
        LPLONG lpPreviousCount  // lReleaseCount로 인해 증가하기전 세마포어 카운트 저장
    );
    // 세마포어 획득
    WaitForSingleObject 함수 호출을 통해 뮤텍스를 획득한다.
    함수를 호출할 때마다 세마포어 카운트가 1 감소한다. 카운트가 0이 되면 세마포어 오브젝트는 Non-Signaled 상태가 된다.
    </code></pre>
    
### 이름있는 뮤텍스
1. 뮤텍스 오브젝트는 커널의 소유이다. 뮤텍스에 이름을 부여하여 다른 프로세스의 쓰레드를 동기화 시킬 수 있다.
2. 관련 함수
    <pre><code>
    // 이름있는 뮤텍스 생성
    HANDLE CreateMutex(
        ...,
        LPCTSTR lpName      // 뮤텍스 이름
    );
    
    // 이미 생성된 뮤텍스에 접근 권한 요청
    HANDLE OpenMutex(
        DWORD dwDesiredAccess,  // 접근 권한 지정(보통 MUTEX_ALL_ACCESS 입력)
        BOOL bInheritHandle,    // 핸들 상속 유무
        LPCTSTR lpName          // 접근 요청 뮤텍스 이름
    );
    
    // 세마포어 획득과 반환은 이름없는 뮤텍스와 동일
    </code></pre>
3. 뮤텍스는 획득한 쓰레드가 직접 반환하는 것이 원칙이다. 이를 지키지 않는다면 다음과 같은 상황이 발생한다.
    <pre><code>
    // A 쓰레드   
    WaitForSingleObject(hMutex); 이후 ReleaseMutex(hMutex)를 하지 않고 쓰레드가 종료됨
    
    // B 쓰레드
    WaitForSingleObject(hMutex) 함수 호출 시 WAIT_ABANDONED가 반환되며 뮤텍스 오브젝트를 획득함
    WAIT_ABANDONED: 뮤텍스 반환이 적절하게 이루어지지 않음을 나타내는 값
    </code></pre>
    * 다른 쓰레드에서 뮤텍스 반환이 이루어지지 않고 종료되어도, **운영체제가** 이를 감지하여 반환시킨다.
