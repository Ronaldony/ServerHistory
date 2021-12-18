# Chapter07 프로세스간 통신(IPC) 1
* 이 장의 주제는 IPC이지만 저자는 IPC를 통해 **커널 오브젝트의 두 가지 상태, 핸들 테이블**에 관한 내용을 설명을 목표로 하고 있다.
## Section01 IPC의 의미 
1. IPC는 Inter-Process-Communication의 약자로서 프로세스들이 데이터를 주고 받는 행위를 의미한다.

### 프로세스들이 서로 만날 수 없는 이유
1. 서로 다른 프로세스들이 접근할수 있는 공유메모리가 있다면 IPC는 굉장히 쉬워질 것이다. 하지만 windows에서는 프로세스들끼리는 메모리 공간을 공유하지 않는다. 프로세스가 접근할 수 있는 영역은 각자에게 제공된 Code, Data, Heap, Stack이기 때문이다.

## Section02 메일슬롯(Mail slot) 방식의 IPC
### 메일슬롯의 원리
1. 메일슬롯은 파이프와 더불어 대표적인 IPC기법이다. Receiver가 특정 주소로 메일슬롯을 생성하면 이 메일슬롯에 Sender가 데이터를 전송하여 Receiver와 통신하는 구조이다.

### 메일슬롯 구성을 위해 필요한 요소
1. 요소
    1) 메일슬롯 생성 (Receiver측)
        <pre><code>
        HANDLE CreateMailslot(
            LPCTSTR lpName,                 // 메일슬롯의 이름 (기본 형식: \\computername\mailslot\[path]name)
            DWORD   nMaxMessageSize,        // 메일슬롯의 버퍼 크기 (0 전달 시 최대 크기로 지정)
            DWORD   lReadTimeout,           // 아래 설명
            LPSECURITY_ATTRIBUTES   lpSecurityAttributes    // 보안 속성(핸들 상속 여부). 
        );
        </code></pre>
        * lReadTimeout:
