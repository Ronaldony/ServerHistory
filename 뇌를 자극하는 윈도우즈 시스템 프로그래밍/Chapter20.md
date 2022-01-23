# Chapter20 메모리 관리(Virtual Memory, Heap, MMF)
## Section01 가상 메모리 컨트롤
### Reserver, Commit 그리고 Free
1. Page는 Reserve(예약), Commit(할당), Free(할당되지 않았음) 세 가지 상태를 갖는다.
    * 여기서 Page는 Stack, Heap 모두 포함한다.
    * Commit: 물리 메모리에 할당된 상태를 말한다. 이 때 물리 메모리는 **RAM과 하드 디스크 모두 포함**
    * Free: 물리 메모리에 전혀 할당이 이뤄지지 않은 영역
    * Reserve: 여러 Page 크기의 메모리 할당 시, 쓰기가 실행되는(실제 사용된) Page 외 나머지 부분을 물리 메모리에 할당되지 않고 예약만 해두는 것이다. 예약 시에 다른 메모리 할당 함수가 해당 Page 영역에 대하여 할당받지 못한다. 이는 연속적인 메모리 사용에 효과적이다. 
2. 페이지의 총 개수 = 가상 메모리의 크기 / 페이지 하나당 크기

### 메모리 할당의 시작점과 단위 확인하기
1. 가상 메모리 시스템에서는 페이지의 중간 위치에서부터 할당을 시작할 수 없으며, 페이지 크기의 배수 다누이로 할당 해야만 한다.
2. Allocation Granularity Boundary(이하 AGB): 메모리 할당의 시작 주소가 될 수 있는 기본 단위
    * 메모리 할당 기본 단위는 페이지이며, 페이지의 배수 단위로 할당이 가능하다.
    * 다음은 Windows10에서의 AGB와 메모리 할당 기본 단위 출력 결과이다.
    <pre><code>
    // Visual Studio 콘솔 출력 결과
    Page size: 4 Kbyte
    allocGranularity: 64 Kbyte
    </code></pre>

### VirutalAlloc & VirtualFree 함수
1. VirtualAlloc
    1) 힙 페이지의 상태를 Reserve와 Commit로 만드는 역할을 한다.
    2) 함수 구조
    <pre><code>
    LPVOID VirtualAlloc(
        LPVOID lpAddress,           // 예약 및 할당 메모리의 시작 주소, 일반적으로 NULL을 전달한다. NULL 전달 시 할당 크기에 맞춰 메모리의 위치를 임의로 결정
                                    // 그러나 Reserve 상태에 있는 페이지인 경우 해당 페이지의 시작 주소를 전달해야 한다. 
                                    // 예약 시 AGB를 기준으로 값이 조절되고, 페이지 크기 단위로 값이 조절된다.
        SIZE_T dwSize,              // 할당 요청 메모리 크기(Byte 단위). 메모리의 할당은 내부적으로 페이지 크기 단위로 변환되어 할당된다.
        DWORD  flAllocationType,    // 메모리 할당 타입을 설정한다. Commit, Reserve 등 8가지의 타입이 있다.
        DWORD  flProtect            // 페이지별 접근방식 제한을 설정한다. 대표적으로 RESERVE 시 PAGE_NOACCESS를, COMMIT 시 PAGE_READWRITE를 인자로 전달한다.
    );
    * 반환: 메모리의 시작 번지를 반환한다. dwSize에 0 전달 시 NULL이 반환됨
    </code></pre>
2. VirtualFree
    1) 힙 페이지의 상태를 Reserve 혹은 Free 상태로 만든다.
    2) 함수 구조
    <pre><code>
    BOOL VirtualFree(
        LPVOID lpAddress,   // 해제할 메모리 시작 주소(반드시 VirtualAlloc에서 반환된 주소여야 한다.) 
        SIZE_T dwSize,      // 해제할 메모리 크기(Byte 단위)
        DWORD  dwFreeType   // MEM_DECOMMIT(Reserve상태) 혹은 MEM_RELEASE(Free상태) 중 하나를 전달한다.
    );
    * dwFreeType에 MEM_RELEASE가 전달되는 경우 dwSize는 무조건 0이 되어야 한다.
    * dwFreeType이 MEM_DECOMMIT인데 dwSize가 할당된 메모리 크기와 다른 경우 함수가 false를 반환한다. 또한, 해당 영역이 그대로 Commit 상태로 남아 있다.
    </code></pre>
    * 물리 메모리에 할당되어 있던 메모리가 반환된다.

## Section02 Heap 컨트롤
### 디폴트 힙(Default Heap) & Windows 시스템에서의 힙
1. 디폴트 힙 혹은 프로세스 힙: 프로세스 생성 시 기본적으로 할당되는 힙, 기본 크기는 1MB이다.

### 디폴트 힙 컨트롤
1. VS 프로젝트 속성->링커->시스템 옵션에서 디폴트 힙 사이즈를 변경할 수 있다. 
    * <img width=600 src="https://user-images.githubusercontent.com/95362065/150666139-e21ba164-ee45-4999-8e10-89dd510dcb8f.PNG">
    * "힙 예약 크기"는 미리 Rserve할 총 크기가 되고 "힙 커밋 크기"는 초기 Commit 크기를 결정한다.
2. 디폴트 힙은 앞서 설명한 옵션에서 크기를 늘리지 않아도, 디폴트 힙을 초과하여 동적 할당 시 Windows 시스템이 알아서 늘려준다.
    * 이럼에도 옵션을 이용한 디폴트 힙 크기 확장의 **장점**은 힙을 확보하는데 런타임 간 시간을 단축시킬 수 있다.

### 동적 힙(Dynamic) 생성이 가져다 주는 또 다른 이점
1. 동적 힙: 디폴트 힙 이외에 Windows 시스템 함수 호출을 통해 생성되는 힙
2. 동적힙의 장점
    1) 장점1: 메모리 단편화 최소화에 따른 성능 향상
        * 디폴트 힙을 사용하여 여러 기능들의 동적 할당이 이루어진다면 각 기능들의 메모리가 뒤섞여 단편화가 발생할 확률이 높다. 단편화가 심해지면 그만큼 프로그램의 로컬리티 특성이 낮아진다.
    2) 장점2: 동기화 문제에서 자유워진다.
        * 여기서 말하는 동기화 문제는 둘 이상의 쓰레드가 같은 주소 번지의 메모리를 할당 및 해제하는 문제를 말한다. 때문에 디폴트 프로세스 힙은 쓰레드가 메모리르 할당 및 해제 시에 내부적으로 동기화 처리를 하고 있다. 하지만 애초에 서로 다른 힙을 생성하여 사용한다면 이 동기화 처리가 필요 없을 것이다.

### 힙의 생성과 소멸 그리고 할당
1. 힙의 생성과 소멸
    1) 힙의 생성
        <pre><code>
        HANDLE HeapCreate(
            DWORD  flOptions,       // 생성되는 힙의 특성 부여. 오류 발생 시 예외 발생, 메모리 할당과 해제에 대한 동기화 처리X 등의 특성을 부여하는 인자
                                    // 동기화 처리x는 HEAP_NO_SERIALIZE 인자를 전달하면 된다.
            SIZE_T dwInitialSize,   // dwMaximumSize에서 지정된 크기 내에서 힙 생성과 동시에 Commit할 크기
            SIZE_T dwMaximumSize    // 힙의 크기를 결정. 이 크기만큼 Reserve 상태가 된다. 0 전달 시에 해당 힙은 증가 가능 메모리가 된다.
                                    // 증가 가능 메모리가 되면 허락하는 한도 내에서 힙의 크기가 계속해서 증가한다.
        );
        * 반환: 생성된 힙 핸들
        </code></pre>
    2) 힙의 소멸
        <pre><code>
        // 핸들에 입력되는 힙 영역 Free 상태로 전환한다
        BOOL HeapDestroy(
            HANDLE hHeap    // 힙 핸들
        );
        </code></pre>
2. 힙 메모리 할당과 해제
    1) 힙 메모리 할당
        <pre><code>
        LPVOID HeapAlloc(
            HANDLE hHeap,       // 힙 핸들
            DWORD dwFlags,      // 옵션 기능. HeapCreate 함수의 flOptions와 유사
            SIZE_T dwBytes      // 할당 메모리 크기, 해당 힙이 증가 가능한 메모리가 아니라면 할당 최대 크기는 0x7FFF8(대략 512KB 크기)이 된다.
                                // 할당 시 페이지 크기 단위로 Commit이 이루어진다.
        );
        </code></pre>
    2) 힙 메모리 해제
        <pre><code>
        BOOL HeapFree(
            HANDLE hHeap,   // 힙 핸들
            DWORD dwFlags,  // 해제 옵션. HEAP_NO_SERIALIZE 입력 시 메모리 해제 시 동기화하지 않음
            LPVOID lpMem    // 해제 메모리 시작 주소
        );
        </code></pre>

## Section03 MMF(Memory Mapped File)
* 생략
