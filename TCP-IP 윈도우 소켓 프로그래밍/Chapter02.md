# Chapter02 윈도우 소켓 시작하기
## Section01 오류 처리
### 오류 처리
1. 윈속 함수의 오류 처리 방법
    1) 오류 처리가 필요없는 경우: 리턴 값이 없거나 호출 시 항상 성공하는 일부 소켓 함수
    2) 리턴 값만으로 오류를 처리하는 경우: WSAStratup()
    3) 리턴 값으로 오류 발생을 확인하고, 구체적인 내용은 오류 코드로 확인하는 경우: 대부분의 소켓 함수.
2. 소켓 함수 호출 오류 코드 출력 함수
    <pre><code>
    // WSAGetLastError
    if (소켓 함수(...) == 실패){
        int errorcode = WSAGetLastError();
        printf("error: %d\n", errorcode);
    }
    </code></pre>
3. 오류 코드 대응 메시지 출력 함수
    <pre><code>
    // 함수 정의
    DWORD FormatMessage(
        DWORD   dwFlags,        // 보통 FORMAT_MESSAGE_ALLOCATE_BUFFER(오류 메시지 저장 공간을 FormatMessage가 알아서 할당), 
                                // FORMAT_MESSAGE_FROM_SYSTEM(운영체제로부터 오류 메시지를 가져옴.)을 OR하여 사용한다.
        LPCVOID lpSource,
        DWORD   dwMessageId,    // 오류 코드, WSAGetLastError()의 리턴 값을 넣는 자리
        DWORD   dwLanguageId,   // 오류 메시지를 표시할 언어 결정
        LPTSTR  lpBuffer,       // 오류 메시지의 시작 주소
        DWORD   nSize,
        va_list *Arguments
    );
    * 주의할 점: 오류 메시지 사용을 마치면 LocalFree 함수를 사용해 오류 메시지 저장을 위해 시스템이 할당한 메모리를 반환해야 한다.
    
    // 소켓 함수 오류 출력 및 종료(Critical한 오류 발생)
    void err_quit(char *msg)
    {
        LPVOID lpMsgBuf;
        FormatMessage(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
            NULL, WSAGetLastError(),
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPSTR)&lpMsgBuf, 0, NULL
        );
        MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
        LocalFree(lpMsgBuf);       
        exit(1);    // 소켓 함수 오류가 발생하였으므로 시스템 종료
    }
    
    // 소켓 함수 오류 출력(사소한 소켓 함수 오류)
    void err_display(char *msg)
    {
        LPVOID lpMsgBuf;
        FormatMessage(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
            NULL, WSAGetLastError(),
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPTSTR)&lpMsgBuf, 0, NULL);
        printf("[%s] %s", msg, (char *)lpMsgBuf);
        LocalFree(lpMsgBuf);
    }
    
    // 함수 사용 예시
    if (socket(...) == INVALID_SOCKET) err_quit("socket()");
    if (socket(...) == INVALID_SOCKET) err_display("socket()");
    </code></pre>

## Section02 윈속 초기화와 종료
### 윈속 초기화와 종료
1. 모든 윈속 응용 프로그램의 공통 구조
    * <img width=150 src="https://user-images.githubusercontent.com/95362065/150976818-14377f30-6c93-412a-a8ab-11d5acafec1d.png">
2. 윈속 초기화: 모든 윈속 프로그램은 소켓 함수를 호출하기 전에 WSAStartup 함수를 호출하여 **윈속 초기화**를 해야한다.
    1) WSAStartup 함수는 사용할 윈속 버전을 요청함으로써 윈속 라이브러리(WS2_32.DLL)를 초기화하는 역할을 한다.
    2) WSAStartup 함수 실패 시 WS2_32.DLL이 메모리에 로드되지 않는다.
    3) WSAStartup 함수 호출 시 윈속 버전이 같아도 실제 사용 가능한 프로토콜은 OS에 따라 달라진다.
    4) WSAStartup 함수 구조
    <pre><code>
    int WSAStartup(
        WORD      wVersionRequired, // 프로그램이 요구하는 최상위 윈속 버전
        LPWSADATA lpWSAData         // WSADATA 구조체를 전달하여 윈도우 운영체제가 제공하는 윈속 구현에 관한 정보를 얻는다.
    );
    </code></pre>
3. 윈속 종료: 프로그램 종료 시 WSACleanup 함수를 호출하여 윈속을 종료해야 한다.
    * WSACleanup 함수는 윈속 사용을 중지함을 OS에 알리고, 관련 리소스를 반환한다.

## Section03 소켓 생성과 닫기
### 소켓 생성
1. 소켓 생성: socket 함수를 통해 소켓을 생성하며 프로토콜을 결정한다.
    1) socket 함수는 해당 프로토콜을 사용할 수 있도록 내부적으로 리소스 할당과 소켓 디스크립터 SOCKET을 리턴한다.
    <pre><code>
    SOCKET WSAAPI socket(
        int af,         // 주소 체계
        int type,       // 소켓 타입
        int protocol    // 프로토콜 타입
    );
    * 반환 성공: 새로운 소켓, 실패: INVALID_SOCKET
    </code></pre>
2. 주소 체계
    * 주소 체계는 네트워크 프로토콜 종류에 따라 달라진다.
    * 주소 체계 인자
    <pre><code>
    #define AF_INET     2   // Internetwork: TCP, UDP 등..
    #define AF_INET6    23  // Internetwork Version 6
    #define AF_IRDA     26  // IrDA
    #define AF_BTH      32  // Bluetooth
    </code></pre>
3. 소켓 타입
    * 사용할 프로토콜의 특성을 나타내는 값이다.
    * 소켓 타입 인자
    <pre><code>
        SOCK_STREAM: 신뢰성 있는 데이터 전송 기능 제공, 연결형 프로토콜
        SOCK_DGRAM: 신뢰성 업는 데이터 전송 기능 제공, 비연결형 프로토콜
    </code></pre>
4. 프로토콜 
    * 주소 체계와 소켓 타입만으로 프로토콜을 결정할 수 있는 경우가 있다. 그러나 일반적으로 프로토콜 인자까지 명시하여야 한다.
5. TCP, UDP 사용 시 socket 함수 인자 전달
    |사용할 프로토콜|주소 체계|소켓 타입|프로토콜|
    |------|---|---|
    |TCP|AF_INET or AF_INET6|SOCK_STREAM|IPPROTO_TCP|
    |UDP|AF_INET or AF_INET6|SOCK_DGRAM|IPPROTO_UDP|
    
### 소켓 닫기
1. 소켓 닫기 함수
    <pre><code>
        int closesocket(
            SOCKET s        // 닫을 소켓 디스크립터
        );
    * 반환 성공:0, 실패: SOCKET_ERROR
    </code></pre>

### WSASocket
1. WSASocket: socket 함수와 역할은 같지만 좀 더 향상된 기능의 함수이다.
    <pre><code>
    SOCKET WSAAPI WSASocketA(
        int                 af,         // socket()과 동일
        int                 type,       // socket()과 동일
        int                 protocol,   // socket()과 동일
        LPWSAPROTOCOL_INFOA lpProtocolInfo,
        GROUP               g,
        DWORD               dwFlags
    );
    </code></pre>
