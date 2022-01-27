# Chapter03 소켓 주소 구조체 다루기
## Section01 소켓 주소 구조체
### 소켓 주소 구조체
1. 소켓 주소 구조체: 네트워크 프로그램에서 필요한 **주소 정보**를 담고 있는 구조체이다. 프로토콜 체계에 따라 주소 지정 방식이 다르므로 다양한 소켓 주소 구조체가 존재한다. 기본이 되는 것은 ws2def.h 파일에 정의된 SOCKADDR 구조체로 다음과 같드 
    <pre><code>
    struct sockaddr {
        ushort  sa_family;      // 주소 체계를 나타냄. 예를 들어 TCP/IP는 AF_INET or AF_INET6
        char    sa_data[14];    // 주소 체계에서 사용할 주소 정보. 예를 들어 TCP/IP의 경우 IP 주소와 포트 번호가 저장됨
    } SOCKADDR;
    </code></pre>
2. 프로토콜 종류마다 사용되는 소켓 주소 구조체가 다른데, TCP/IP에서는 SOCKADDR_IN or SOCKADDR_IN6를 IrDAT에서는 SOCKADDR_IRDA를 사용한다.
    <pre><code>
    // SOCKADDR_IN, ws2def.h에 정의
    typedef struct sockaddr_in {
        short          sin_family;      // 주소 체계를 나타냄. AF_INET
        USHORT         sin_port;        // 포트 번호 지정
        IN_ADDR        sin_addr;        // IP 주소 시정
        CHAR           sin_zero[8];     // 0으로 설정
    } SOCKADDR_IN, *PSOCKADDR_IN;
    
    // SOCKADDR_IN6, ws2ipdef.h에 정의
    struct sockaddr_in6 {
        short sin6_family;          // 주소 체계를 나타냄. AF_INET6
        u_short sin6_port;          // 포트 번호 지정
        u_long sin6_flowinfo;        // 대부분 0으로 설정
        struct in6_addr sin6_addr;   // IP 주소 시정
        u_long sin6_scope_id;       // 대부분 0으로 설정
    };
    </code></pre>
3. in_addr과 in6_addr
    <pre><code>
    // in_addr, inaddr.h에 정의
    struct in_addr {
        union {
            struct { u_char s_b1; u_char s_b2; u_char s_b3; u_char s_b4; } S_un_b;
            struct { u_short s_w1; u_short s_w2; } S_un_w;
            u_long S_addr;
        } S_un;
    } IN_ADDR;
    
    // in6_addr, in6addr.h에 정의
    typedef struct in6_addr {
        union {
            UCHAR  Byte[16];
            USHORT Word[8];
        } u;
    } IN6_ADDR, *PIN6_ADDR, *LPIN6_ADDR;
    </code></pre>
4. 프로토콜별 소켓 주소 구조체
    |프로토콜|구조체 이름|전체 크기(Byte 단위)|
    |------|---|---|
    |Default|SOCKADDR|16|
    |TCP, UDP IPv4|SOCKADDR_IN|16|
    |TCP, UDP IPv6|SOCKADDR_IN6|28|
    |IrDA|SOCKADDR_IRDA|32|
    |Bluetooth|SOCKADDR_BTH|30|
    
## Section02 바이트 정렬 함수
* 이 절에서는 네트워크 통신에서 바이트ㅡ 정렬 방식을 고려해야 하는 경우를 프로토콜 구현과 응용 프로그램 데이터라는 두 가지 관점에서 살펴본다.
### 프로토콜 구현 관점에서의 바이트 정렬
1. IP 관점: 한 호스트에서 보낸 Frame의 **IP 헤더**가 의 바이트 정렬 방식이 **라우터**의 바이트 정렬 방식과 일치하지 않은 경우 정상적인 목적지에 전달될 수 없다.
2. 포트 번호 관점: 통신을 하는 **두 호스트 간의 포트 번호** 바이트 정렬 방식이 일치하지 않으면 잘못된 프로세스와 연결이 될 수 있다.
3. 위 두 문제를 해결하기 위하여 네트워크에서는 빅 엔디안을 통일해 사용한다. 이를 **네트워크 바이트 정렬**이라 부른다.
