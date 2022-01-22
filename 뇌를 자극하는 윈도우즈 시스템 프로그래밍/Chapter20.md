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
