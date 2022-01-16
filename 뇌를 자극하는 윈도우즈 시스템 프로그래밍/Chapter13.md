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
