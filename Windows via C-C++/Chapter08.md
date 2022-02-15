# Chapter08 유저 모드에서의 스레드 동기화
## Section01 원자적 접근: Interlocked 함수들
### 원자적 접근
1. 원자적 접근: 어떤 스레드가 특정 리소스를 접근할 때 다른 스레드는 동일 시간에 동일 소스에 접근할 수 없는 것
2. MS는 다양한 인터락 계열 함수를 지원한다.
    * 인터락 함수들은 CPU 플랫폼마다 동작 원리가 다르다. x86 계열의 CPU는 인터락 함수들이 버스에 하드웨어 시그널을 실어 다른 CPU가 동일 메모리 주소에 접근하지 못하도록 한다.
    * 인터락 함수에 전달하는 주소 값은 Alignment 되어 있어야 한다. 그러지 않은 경우 함수 호출 실패
3. 인터락 함수의 장점
    1) 매우 빠르게 동작
    2) 유저<->커널 모드 전환이 발생되지 않음
4. 윈도우 XP 이후로는 인터락 싱글 링크드 리스트(Interlocked Singly Linked List) 스택을 제공한다.
    * 이 스택은 값을 Push & Pop 동작을 통해 원자적으로 해당 값에 접근한다.

### 스핀락
1. 인터락 함수를 사용한 스핀락 구현
    <pre><code>
    // 리소스의 접근을 기다림
    while (InterlockedExchange(&g_num, TRUE) == TRUE)
        Sleep(0);
    // 리소스에 접근..
    
    // 리소스에 더 이상 접근 필요 없음
    InterlockedExchange(&g_num, FALSE);        
    * InterlockedExchange 반환: TRUE인 경우 해당 리소스가 사용 중임을 나타낸다. 반대로 FALSE인 경우 해당 리소스는 사용 중이 아니다.
    </code></pre>
2. 스핀락 사용 시 주의 사항
    1) 스핀락을 사용하는 모든 스레드는 우선순위 레벨이 동일하고, 스레드 우선 순위 동적 상승 기능이 불가능하도록 설정되어야 한다.
    2) 락 변수와 공유 리소스는 서로 다른 캐시 라인에 있도록 하는 것이 좋다. 만약 락 변수와 공유 리소스가 동일한 캐시 라인에 있다면, 리소스를 사용 중인 CPU는 동일 리소스에 접근하고자 하는 다른 CPU와 경쟁하게 될 것이다.
3. 효율적인 스핀락 구현
    1) 공유 리소스가 다른 스레드에 의해 사용중인 경우(=InterlockedExchange 결과가 TRUE인 경우) 일정 횟수만큼 시도한 후, 그래도 사용 중이라면 커널 모드로 전환하여 리소스가 가용해질 때 까지 대기 상태를 유지하는 것이 좋다. (이는 크리티컬 섹션의 구현 방식임)

## Section02 캐시 라인
### 캐시 라인
1. 다른 CPU가 캐시 라인에 있는 정보를 변경하면 다른 CPU는 자신의 캐시 라인에 있는 **정보를 무효화** 시킨다.
    * 이로 인해 멀티프로세서 머신에서는 거꾸로 성능을 저해하는 요인이 될 수도 있다!!
2. 캐시 히트율 향상 방법
    1) 데이터별로 개별적으로 캐시 라인의 경계에 스게하는 것
    2) 읽기 전용과 읽고 쓰는 데이터를 분리하는 것
    3) 동일한 시간에 접근하는 데이터들을 묶는 것
3. 캐시 히트율 향상 구조 예시
    1) 나쁜 예
    <pre><code>
    struct CUSTINFO{
        DOWRD id;           // 거의 읽기 전용
        int balance;        // 읽고 쓰기
        wchar_t szName;     // 거의 읽기
        FILETIME ft;        // 읽고 쓰기
    };
    </code></pre>
    2) 좋은 예
    <pre><code>
    struct alignas(CACHE_LINE) CUSTINFO{
        wchar_t szName;     // 거의 읽기
        DOWRD id;           // 거의 읽기 전용
        
        alignas(CACHE_LINE) int balance;        // 읽고 쓰기
        FILETIME ft;        // 읽고 쓰기
        
    };
    </code></pre>

## Section03 고급 스레드 동기화 기법
### 회피 기술
1. 이 책에서 소개하는 회피 기술은 Polling 방법이다. 되도록이면 스핀락이나 폴링 방법으로 동기화하는 것은 하지 않는 것이 좋다.

## Section04 크리티컬 섹션
### 크리티컬 섹션
1. 크리티컬 섹션 구조체
    <pre><code>
    struct _RTL_CRITICAL_SECTION {
        PRTL_CRITICAL_SECTION_DEBUG DebugInfo;
        LONG LockCount;
        LONG RecursionCount;
        HANDLE OwningThread;        // from the thread's ClientId->UniqueThread
        HANDLE LockSemaphore;
        ULONG_PTR SpinCount;        // force size on 64-bit systems when packed
    } RTL_CRITICAL_SECTION, *PRTL_CRITICAL_SECTION;
    
    // 실제로는 typdef된 CRITICAL_SECTION을 사용
    typedef RTL_CRITICAL_SECTION CRITICAL_SECTION;
    
    * RTL_CRITICAL_SECTION 구조체는 MSDN에 문서화되어 있지 않다. MS의 의도이다.
    </code></pre>
2. 크리티컬 섹션 함수
    <pre><code>
    // 크리티컬 섹션 구조체 초기화
    void InitializeCriticalSection(
        LPCRITICAL_SECTION lpCriticalSection    // CRITICAL_SECTION 구조체 주소
    );
    // 크리티컬 섹션 구조체 삭제
    void DeleteCriticalSection(
        LPCRITICAL_SECTION lpCriticalSection
    );
    // 크리티컬 섹션 진입
    void EnterCriticalSection(
        LPCRITICAL_SECTION lpCriticalSection
    );
    // 크리티컬 섹션 탈출
    void LeaveCriticalSection(
        LPCRITICAL_SECTION lpCriticalSection
    );
    </code></pre>
3. EnterCriticalSection 내부 동작
    1) 다른 스레드가 이미 공유 리소스에 대한 접근 권한을 획득한 상태라면, 이벤트 커널 오브젝트를 이용하여 스레드가 Blcoked 상태가 된다. 이후 공유 리소스가 사용 가능해지면 해당 스레드를 Ready 상태로 전환한다.
    2) 공유 리소스를 사용하는 스레드가 없다면 LPCRITICAL_SECTION 구조체 내 멤버변수를 갱신하여 해당 스레드가 권한을 획득했음을 설정한 후 지체없이 함수를 반환한다.
    3) EnterCriticalSection 호출한 스레드는 지정된 시간이 만료되어도 공유 리소스를 사용하지 못하는 경우 예외를 발생시킨다.(만료 시간은 레지스트리 키를 통해 변경할 수 있음)
4. TryEnterCriticalSection
    1) 기능: 크리티컬 섹션 전급이 가능한 경우 TRUE를, 불가능한 경우 FALSE를 반환한다.
    2) 주의할 점: TRUE가 반환되는 경우 현재 스레드가 공유 리소스의 접근 권한을 획득한 것으로 갱신하게 된다. 따라서 공유 리소스 사용 후 LeaveCriticalSection를 호출해야 함
    <pre><code>
    // 크리티컬 섹션 접근 가능 여부
    BOOL TryEnterCriticalSection(
        PCRITICAL_SECTION pcs
    );
    </code></pre>

### 크리티컬 섹션과 스핀락
1. **크리티컬 섹션의 문제**: 크리티컬 섹션 내 공유 리소스가 사용중인 경우 진입을 시도한 다른 스레드들은 Blocked 상태가 되기 위해 유저 모드->커널 모드로 전환하게 된다. 그런데 커널 모드로 전환 중에 공유 리소스의 소유권이 반환되는 경우 커널 모드로의 전환을 시도한 스레드의 CPU 할당이 낭비가 되어버린다. 이러한 점을 개선하기 위해 MS는 크리티컬 섹션에 스핀락 매커니즘을 도입하였다.
2. 스핀락 매커니즘이 도입된 크리티컬 섹션
    <pre><code>
    // 스핀락 매커니즘이 도입된 크리티컬 섹션 구조체 초기화
    BOOL InitializeCriticalSectionAndSpinCount(
        LPCRITICAL_SECTION lpCriticalSection,   // CRITICAL_SECTION 구조체 주소
        DWORD              dwSpinCount          // 스핀락 루프 횟수
    );
    * 반환: 함수 내부적으로 디버깅 정보를 위한 메모리 블럭 할당 성공 시 TRUE, 실패 시 FALSE
    // 스핀 횟수 변경
    DWORD SetCriticalSectionSpinCount(
        LPCRITICAL_SECTION lpCriticalSection,
        DWORD              dwSpinCount
    );
    </code></pre>
3. 본 책에서는 일반 크리티컬 섹션보다 **스핀락 매커니즘이 도입된 크리티컬 섹션 사용을 권한다**.
4. Tip: 프로세스 힙에 대한 접근을 보호하기 위해 사용하는 크리티컬 섹션의 스핀 카운트는 대략 4000이다.

### 크리티컬 섹션과 에러 처리
1. InitializeCriticalSection 함수는 내부적으로 디버깅 정보를 위한 메모리 블럭을 할당하는데, 메모리 블럭 할당을 실패하는 경우도 있다. 이는 InitializeCriticalSectionAndSpinCount 함수를 사용하여 대체할 수 있다.(위 함수 설명 자세히보기)
2. 둘 이상의 스레드가 동일 시간에 크리티컬 섹션 진입 경쟁을 하는 경우 이벤트 커널 오브젝트를 생성 시, 생성에 실패할 수 있다.
