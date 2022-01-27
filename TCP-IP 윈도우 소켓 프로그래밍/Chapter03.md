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
1. IP 관점: 한 호스트에서 보낸 Frame의 **IP 헤더** 바이트 정렬 방식이 **라우터**의 바이트 정렬 방식과 일치하지 않은 경우 정상적인 목적지에 전달될 수 없다.
2. 포트 번호 관점: **두 호스트 간의 포트 번호** 바이트 정렬 방식이 일치하지 않으면 잘못된 프로세스와 연결이 될 수 있다.
3. 위 두 문제를 해결하기 위하여 네트워크에서는 빅 엔디안을 통일해 사용한다. 이를 **네트워크 바이트 정렬**이라 부른다.
4. 호스트 바이트 정렬: 시스템이 사용하는 바이트 정렬 방식

### 응용 프로그램 데이터 관점에서의 바이트 정렬
1. 데이터 관점: 두 호스트간 바이트 정렬 방식을 약속하지 않으면 데이터 해석 문제가 발생할 수 있다. 대개는 클라이언트가 서버의 바이트 정렬을 따른다.

### 바이트 정렬 함수
1. 응용 프로그램이 바이트 정렬 방식을 편리하게 변환할 수 있도록 다음과 같은 윈속 함수가 제공된다.
    <pre><code>
    // short 타입 호스트 바이트 정렬 -> 네트워크 바이트 정렬 변환
    u_short htons( u_short hostshort );
    // long타입 htons
    u_long htonl( u_long hostlong );
    
    // short 타입 네트워크 바이트 정렬 -> 호스트 바이트 정렬 변환
    u_short ntohs( u_short netshort );
    // long타입 ntohs    
    u_long ntohl( u_long netlong );
    </code></pre>
2. 바이트 정렬 확장 함수: WSAHtons, WSAHtonl, WSANtohs, WSANtohl가 있다.
3. SOCKADDR_IN과 SOCKADDR_IN6 구조체의 바이트 정렬
    * <img width=600 src="https://user-images.githubusercontent.com/95362065/151297598-5bc0febe-562a-46e7-9d39-e5df2ace8280.png">

## Seciont03 IP 주소 변환 함수
### IP 주소 변환 윈속 함수
1. IPv4 주소 변환
    <pre><code>
    // IPv4 주소 변환
    // 문자열 -> 숫자
    unsigned long inet_addr(const char *cp);
    
    // 숫자 -> 문자열
    char *inet_ntoa(struct in_addr in);
    </code></pre>
2. IPv4 + IPv6 주소 변환
    <pre><code>
    // IPv4 or IPv6 주소 변환
    // 문자열 -> 숫자
    INT WSAAPI WSAStringToAddressA(
        LPSTR               AddressString,      // 문자열 형식의 IP 주소
        INT                 AddressFamily,      // AF_INET or AF_INET6
        LPWSAPROTOCOL_INFOA lpProtocolInfo,     // NULL
        LPSOCKADDR          lpAddress,          // IP 주소(숫자)를 저장할 구조체; SOCKADDR_IN or SOCKADDR_IN6
        LPINT               lpAddressLength     // 주소 구조체의 길이
    );
    // 숫자 -> 문자열
    INT WSAAPI WSAAddressToStringA(
        LPSOCKADDR          lpsaAddress,                // 숫자 형식의 IP 주소; SOCKADDR_IN or SOCKADDR_IN6
        DWORD               dwAddressLength,            // 주소 구조체의 길이
        LPWSAPROTOCOL_INFOA lpProtocolInfo,             // NULL
        LPSTR               lpszAddressString,          // IP 주소(문자열)을 저장할 버퍼
        LPDWORD             lpdwAddressStringLength     // 버퍼의 길이
    );
    
    // 사용 예시
    char* ipv6test = "0:0:0:0:0:0:0:1"; // 루프백 주소
    SOCKADDR_IN6 ipv6num;
    int addrlen = sizeof(ipv6num);
    // 문자열 -> 숫자
    WSAStringToAddressA(ipv6test, AF_INET6, NULL, (SOCKADDR*)&ipv6num, &addrlen);
    
    char ipaddr[50];
    DWORD ipaddrlen = sizeof(ipaddr);
    // 숫자 -> 문자열
    WSAAddressToStringA((SOCKADDR*)&ipv6num, sizeof(ipv6num), NULL, ipaddr, &ipaddrlen);    
    </code></pre>

### Section04 도메인 이름 시스템과 이름 변환 함수
### 도메인
1. 도메인 이름(domain name)은 IP 주소와 마찬가지로 호스트 혹은 라우터의 고유한 식별자이다.
2. TCP/IP 프로토콜은 내부적으로 숫자 형태의 IP 주소를 기반으로 동작하므로 사용자가 입력한 도메인 이름을 반드시 IP 주소로 변환해야 한다.
3. 도메인 이름과 IP 주소의 변환 정보는 **DNS 서버**가 관리한다.

### 도메인 이름과 IP 주소 변환
1. 도메인<->IP주소 변환 함수
    <pre><code>
    // 도메인 -> IP 주소(네트워크 바이트 정렬)
    struct hostent *gethostbyname(
        const char* name    // 도메인 이름
    );
    
    // IP 주소(네트워크 바이트 정렬) -> 도메인
    struct hostent *gethostbyaddr(
        const char *addr,   // IP 주소 (네트워크 바이트 정렬)
        int len,            // IP 주소의 길이
        int type            // 주소 체계(AF_INET or AF_INET6)
    );
    </code></pre>
2. hostent 구조체
    1) hostent의 구조
    <pre><code>
    typdef struct hostent {
        char*   h_name;         // 공식 도메인 이름
        char**  h_aliases;      // 한 호스트가 공식 도메인 외 다른 이름을 가질 수 있는데, 이를 별명(Aliases)이라 한다. 이 별명의 배열이다.
        short   h_addrtype;     // 주소 체계, AF_INET or AF_INET6가 저장
        short   h_length;       // IP 주소의 길이(바이트 단위), 4(Ipv4) 또는 16(IPv6)이 저장
        char**  h_addr_list;    // IP 주소, 한 호스트가 여러 IP 주소를 가진 경우 IP 주소 배열 형태로 접근할 수 있다.
    #define h_addr h_addr_list[0]
    } HOSTENT;
    </code></pre>
    * <img width=700 src="https://user-images.githubusercontent.com/95362065/151317073-d0cc556f-8084-48d5-9686-08c0e7830186.jpg">
    2) 실제 사용 예시
    <pre><code>
    // 도메인으로 IPv4 주소 얻기
    HOSTENT* ptr = gethostbyname(도메인 이름);
    
	if (ptr == NULL)    
		printf("gethostbyname 함수 에러\n");	
	if(ptr->h_addrtype != AF_INET)
		printf("Domain is not internet protocol\n");
        
	IN_ADDR addr;
	memcpy(&addr, ptr->h_addr, ptr->h_length);
	printf("addr: %x\n", addr.S_un.S_addr);
    
    // IPv4 주소 -> 도메인
    IN_ADDR addr;
	HOSTENT* ptr = gethostbyaddr((char*)&addr, sizeof(addr), AF_INET);
    
	if (ptr == NULL)
		printf("gethostbyaddr 함수 에러\n");
	if (ptr->h_addrtype != AF_INET)
		printf("Domain is not internet protocol\n");

	char name[50] = {0,};
	memcpy(name, ptr->h_name, sizeof(name));
	printf("addr: %s\n", name);
    </code></pre>
3. 도메인 이름 -> IP주소 변환은 항상 가능하지만, 그 역은 아니다. 따라서 IP 주소를 도메인으로 변환한 결과는 참고로만 사용하는 것이 좋다.
4. HOSTENT 구조체는 스레드당 하나씩 할당된다. 
