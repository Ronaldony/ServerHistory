# Chapter14 가상 메모리 살펴보기
## Section01 시스템 정보
### 시스템 정보
1. 운영체제 구성 정보와 관련된 값들(페이지 크기, 할당 단위 크기 등)을 소스 코드에서 사용할 때에는 절대 하드코딩해서는 안 된다.
  * 따라서, GetSystemInfo 함수를 이용하여 현재 시스템의 구성 정보와 관련된 값들을 가져와 사용하여야 한다.

### GetSystemInfo 함수
1. GetSystemInfo 함수는 입력된 SYSTEM_INFO 구조체에 적절한 값을 채워 반환한다.
    1) SYSTEM_INFO 구조체
    <pre><code>
    typedef struct _SYSTEM_INFO {
        union {
            DWORD dwOemId;
            struct {
                WORD wProcessorArchitecture;
                WORD wReserved;
            } DUMMYSTRUCTNAME;
        } DUMMYUNIONNAME;
        DWORD     dwPageSize;                       // CPU의 페이지 크기(x86, x64에서는 4KB)
        LPVOID    lpMinimumApplicationAddress;      // 각 프로세스가 사용할 수 있는 가장 작은 주소 값(64KB)
        LPVOID    lpMaximumApplicationAddress;      // 각 프로세스가 사용할 수 있는 가장 큰 주소 값
        DWORD_PTR dwActiveProcessorMask;            // 사용 가능한 CPU를 가리키는 비트마스크(스레드를 수행할 수 있는 CPU)
        DWORD     dwNumberOfProcessors;             // CPU의 개수
        DWORD     dwProcessorType;                  
        DWORD     dwAllocationGranularity;          // 가상 주소 공간에서 특정 영역을 예약할 때 사용하는 단위 크기
        WORD      wProcessorLevel;                  // 프로세서의 아키텍처를 세분화한 값
        WORD      wProcessorRevision;               // 프로세서의 레벨을 세분화한 값
    } SYSTEM_INFO, *LPSYSTEM_INFO;
    </code></pre>

## Section03 NUMA 머신에서의 메모리 관리
### NUMA 머신
1. NUMA 머신 구조에서는 CPU가 자신의 노드뿐만 아니라 다른 노드의 메모리에도 접근이 가능하다. 하지만 CPU가 다른 노드에 속한 메모리에 접근하는 것보다 자신의 노드에 속한 메모리에 접근하는 것이 월등히 빠르다.
