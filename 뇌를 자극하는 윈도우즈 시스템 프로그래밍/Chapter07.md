# Chapter07 프로세스간 통신(IPC) 1
* 이 장의 주제는 IPC이지만 저자는 IPC를 통해 **커널 오브젝트의 두 가지 상태, 핸들 테이블**에 관한 내용을 설명을 목표로 하고 있다.
## Section01 IPC의 의미 
1. IPC는 Inter-Process-Communication의 약자로서 프로세스들이 데이터를 주고 받는 행위를 의미한다.

### 프로세스들이 서로 만날 수 없는 이유
1. 서로 다른 프로세스들이 접근할수 있는 공유메모리가 있다면 IPC는 굉장히 쉬워질 것이다. 하지만 windows에서는 프로세스들끼리는 메모리 공간을 공유하지 않는다. 프로세스가 접근할 수 있는 영역은 각자에게 제공된 Code, Data, Heap, Stack이기 때문이다.

## Section02 메일슬롯(Mail slot) 방식의 IPC
### 메일슬롯의 원리
1. 메일슬롯은 파이프와 더불어 대표적인 IPC기법이다. Receiver가 특정 주소로 메일슬롯을 생성하면 이 메일슬롯에 Sender가 데이터를 전송하여 통신하는 구조이다.
2. 메일슬롯 생성 시 커널 오브젝트와 커널 오브젝트를 가리키는 핸들을 반환받는다.

### 메일슬롯 구성을 위해 필요한 요소
1. Receiver
    1) 메일슬롯 생성
        <pre><code>
        HANDLE CreateMailslot(
            LPCTSTR lpName,                 // 메일슬롯의 이름 or 주소 (기본 형식: \\computername\mailslot\[path]name). 조금 더 상세한 내용은 아래에
            DWORD   nMaxMessageSize,        // 메일슬롯의 버퍼 크기 (0 전달 시 최대 크기로 지정)
            DWORD   lReadTimeout,           // 아래 설명
            LPSECURITY_ATTRIBUTES   lpSecurityAttributes    // 보안 속성(핸들 상속 여부). 
        );
        </code></pre>
        * lReadTimeout: 메일슬롯은 파일 입출력 기반으로 만들어져 Sender는 WriteFile, Receiver는 ReadFile 함수로 메일슬롯의 데이터 입출력이 일어난다. 이 lReadTimeout은 메일슬롯을 ReadFile 로 읽어들일 경우의 최대 블로킹 시간을 ms 단위로 지정한다. (0:넌블로킹, MAILSLOT_WAIT_FOREVER: 무한 블로킹)
        * computername: 네트워크 도메인을 입력한다. '.' 입력 시 로컬 컴퓨터를 의마하고, '*' 입력 시 같은 도메인의 모든 PC를 의미함
        * [path]name: 실질적인 메일슬롯 이름이다. \aaa\bbb\mailslot 혹은 \mailslot 과 같이 사용자 임의 형태로 지정할 수 있다.
    2) 메일슬롯 읽기
        * ReadFile 함수
2. Sender
    1) 메일슬롯 데이터 쓰기
        * WriteFile 함수

###  메일슬롯 예제
* 본 디렉터리 Chapter07_src/MailslotReceiver.cpp, Chapter07_src/MailslotSender.cpp 참고

### 메일슬롯의 고찰
1. 메일슬롯은 Sender->Receiver로만 전송하는 단방향 통신이다. (Receiver->Sender로 데이터 전송 불가능)
    1) Sender에서 Mailslot 생성 후 Receiver가 해당 Mailslot으로 ReadFile 시 함수 fail
    2) Sender에서 Mailslot 생성 후 Sender가 해당 Mailslot으로 WriteFile 시 함수 fail
2. 메일슬롯은 브로드캐스팅 방식의 통신을 지원한다. 브로드캐스팅 방식은 하나의 Sender가 동시에 여러 Receiver에게 메시지를 전달할 수 있다.

## Section03 Signaled vs Non-Signaled
### 커널 오브젝트의 두 가지 상태
1. 커널 오브젝트는 Signaled, Non-Signaled 이 두 가지 상태를 갖는다. Signaled에서 Non-Signaled 상태로 그 반대로도 상태가 변경될 수 있다.
2. 상태 정보는 커널 오브젝트에 저장된다. Signaled는 TRUE, Non-Signaled는 FALSE 값을 갖는다.
3. 상태는 특정 상황마다 변경되는데 각 커널 오브젝트마다(리소스마다) 변경되는 상황이 다르다. 
    * 프로세스 커널 오브젝트는 최초 생성 시 Non-Signaled 상태를 갖다가 프로세스가 종료되면 Signaled 상태로 변경된다. 종료된 프로세스를 다시 실행시킬 수 있는 방법이 없으므로 다시 Non-Signaled 상태로 되돌아갈 수 없다.

### 커널 오브젝트의 두 가지 상태를 확인하는 용도의 함수
1. 커널 오브젝트의 상태를 지니도록 한 것은 windows가 프로그래머에게 상태를 활용한 다양한 기능을 제공하기 위해서다.
2. 커널 오브젝트 상태 관련 함수
    1) 커널 오브젝트의 상태 정보 확인 함수
    <pre><code>
    DWORD WaitForSingleObject(      // 기능: 블로킹 함수로 hHandle에 해당하는 커널 오브젝트가 signaled 상태일 때 함수를 반환된다.
        HANDLE hHandle,             // 커널 오브젝트의 핸들
        DWORD  dwMilliseconds       // 최대 블로킹할 ms (INFINITE: signaled 상태가 될 때까지 블로킹)
    );
    DWORD WaitForMultipleObjects(   // 기능: 블로킹 함수로 lpHandles에 해당하는 커널 오브젝트들이 signaled 상태일 때 함수를 반환된다
        DWORD        nCount,        // 검사할 커널 오브젝트 수
        const HANDLE *lpHandles,    // 커널 오브젝트들의 핸들이 저장된 배열의 주소
        BOOL         bWaitAll,      // TRUE: lpHandles의 모든 커널 오브젝트 상태가 signaled일 때 함수 반환, FALSE: 하나라도 signaled일 떄 반환
        DWORD        dwMilliseconds // 최대 블로킹할 ms (INFINITE: signaled 상태가 될 때까지 블로킹)
    );
    </code></pre>    
    * 출처: https://docs.microsoft.com/en-us/windows/win32/api/synchapi/nf-synchapi-waitformultipleobjects
    2) 사용 예시
        * <img width="600" src="https://user-images.githubusercontent.com/95362065/146649370-0c1460fd-adb8-45d6-a5d7-4eb980cc6a25.png">
    3) 예제
        * 본 디렉터리 Chapter07_src/WatiForObject.cpp, Chapter07_src/WatiForObject2.cpp 참고
