# Chapter08 프로세스간 통신(IPC) 2
## Section01 핸들 테이블과 오브젝트 핸들의 상속
### 프로세스의 커널 오브젝트 핸들 테이블
1. 프로세스들은 커널 오브젝트들에 접근하는 **핸들 테이블을 독립적**으로 가지게 된다. 이 핸들 테이블은 간략하게 다음과 같이 구성된다.
    *   |핸들|주소(커널 오브젝트 할당 주소)|상속여부|
        |---|---|---|
        |30|0x1000|Y|
        |50|0x1100|N|
        |...|...|...|
    * 핸들의 값은 각 프로세스마다 독립적이다. 다른 두 프로세스의 같은 핸들 값이 같은 커널 오브젝트를 의미하지 않는다!!
2. 핸들 테이블 등록과 삭제
    1) 등록: CreateProcess나 CreateMailslot과 같은 리소스 생성을 요청하면 핸들 테이블에 해당 리소스의 핸들 정보가 등록된다.
    2) 삭제: CloseHanle 함수 호출 시 인자로 입력된 리소스의 핸들 정보가 핸들 테이블에서 삭제된다.
3. 핸들 테이블은 프로세스별로 독립적이지만 **운영체제에 의해 관리**되는 하나의 리소스이다.

### 핸들의 상속
1. 부모 프로세스는 자식 프로세스에게 자신의 핸들 테이블을 상속시킬 수 있다. 핸들 테이블이 상속되는 예시는 다음과 같다.
    *   부모의 핸들 테이블
        |핸들|주소(커널 오브젝트 할당 주소)|상속여부|
        |---|---|---|
        |30|0x1000|Y|
        |50|0x1100|N|
        |...|...|...|
    *   자식의 핸들 테이블
        |핸들|주소(커널 오브젝트 할당 주소)|상속여부|
        |---|---|---|
        |30|0x1000|Y|
        |...|...|...|
2. 자식에게 상속된 핸들 테이블은 자식 프로세스가 또 다른 자식 프로세스를 생성하여도 그대로 상속된다.
3. 핸들의 상속을 위한 전달인자
    1) bInheritHandles
    <pre><code>
     BOOL CreateProcess(
     ...,
     BOOL   bInheritHandles,    // TURE: 생성되는 자식 프로세스에게 핸들 테이블에서 상속여부 'Y'인 핸들을 상속시킴, FALSE: 핸들 테이블 상속시키지 않음.
     ...
     );

    </code></pre>

### 핸들 테이블과 Usage Count
1. 프로그래머가 리소스 생성 요청 후, 핸들 테이블에 핸들이 등록되는 순간 해당 커널 오브젝트의 Usage Count가 1 증가한다.

### 상속이 되기 위한 핸들의 조건
1. 상속여부가 'Y'가 되어야 핸들은 자식 프로세스에게 상속된다. 이는 프로그래머에 의해 결정된다.
    1) SECURITY_ATTRIBUTES 구조체
    <pre><code>
     HANDLE CreateMailslot(     // 메일슬롯 생성 함수
        ...
        LPSECURITY_ATTRIBUTES   lpSecurityAttributes    // 보안 속성 구조체 포인터 
    );
    typedef struct _SECURITY_ATTRIBUTES {   // 보안 속성 구조체
        DWORD  nLength;                     // 구조체 변수 크기
        LPVOID lpSecurityDescriptor;        // 설명 생략. 핸들의 상속 관점에서 의미 없음
        BOOL   bInheritHandle;              // TRUE: 생성되는 리소스의 핸들 상속여부를 'Y'로 지정, FALSE: 상속여부를 'N'으로 지정
    } SECURITY_ATTRIBUTES, *PSECURITY_ATTRIBUTES, *LPSECURITY_ATTRIBUTES;
    </code></pre>
    
### Pseudo 핸들과 핸들의 중복(Duplicate)
1. 가짜 핸들: 임의 프로세스에서 GetCurrentProcess 함수 호출을 통하여 얻는 핸들은 가짜 핸들이다. 이유는 이 핸들의 값이 -1로 고정되어 있고 핸들 테이블에 등록되어 있지 않기 때문이다. (따라서, 자식에게 상속될 수 없음)
2. 핸들 복제
    1) windows는 가짜 핸들이나 진짜 핸들을 핸들 테이블에 **복제**하는 기능을 지원한다.
    2) DuplicateHandle 함수
    <pre><code>
    BOOL DuplicateHandle(                   // 기능: 지정된 핸들을 핸들 테이블에 중복 등록한다.
        HANDLE   hSourceProcessHandle,      // 복제 할 핸들을 소유한 프로세스 
        HANDLE   hSourceHandle,             // 복제 할 핸들
        HANDLE   hTargetProcessHandle,      // 복제된 핸들을 소유할 프로세스
        LPHANDLE lpTargetHandle,            // 복제된 핸들을 저장할 주조
        DWORD    dwDesiredAccess,           // 복제된 핸들의 접근권한. (Default:0, 무시: DUPLICATE_SAME_ACCESS)
        BOOL     bInheritHandle,            // 복지된 핸들의 상속여부
        DWORD    dwOptions                  // 접근권한 옵션
    );
    </code></pre>
    * 복제되는 핸들의 값은 원본의 값과 무관하다.

### 예제, 부모 프로세스의 핸들을 자식 프로세스에게 전달하기
* 본 디렉터리 Chapter08_src/DuplicateProcessChild.cpp, Chapter08_src/DuplicateProcessParent.cpp 참고
* 실행: 각 .cpp를 소스코드 이름과 동일하게 실행파일을 만든 후 DuplicateProcessParent.exe 실행

## Section02 파이프 방식의 IPC
### 메일슬롯에 대한 회고와 파이프의 이해
1. 메일슬롯은 서로 관련이 없는 프로세스들(부모 자식이 아닌)간에 통신 시 유용하다.
2. 파이프는 Nameed 파이프, Anonymous 파이프 두 가지가 있다.
3. 메일슬롯과 파이프 비교
    1) 메일슬롯 :           브로드캐스트 방식, 단방향 통신(Send->Receiver), 프로세스 부모 자식 관계 상관없는 통신(주소를 이용한 통신)
    2) Anonymous 파이프:   단방향 통신, 부모 자식 프로세스간에 통신
    3) Named 파이프:       양방향 통신, 프로세스 부모 자식 관계 상관없는 통신(주소를 이용한 통신)

### Anonymous 파이프
1. 함수
    1) Anonymous 파이프 생성
        <pre><code>
        BOOL CreatePipe(
            PHANDLE               hReadPipe,            // 읽기 파이프 핸들 저장 주소
            PHANDLE               hWritePipe,           // 쓰기 파이프 핸들 저장 주소
            LPSECURITY_ATTRIBUTES lpPipeAttributes,     // 보안 속성
            DWORD                 nSize                 // 파이프의 버퍼 사이즈(0 입력 시 디폴트 버퍼)
        );
        * 출처: https://docs.microsoft.com/en-us/windows/win32/api/namedpipeapi/nf-namedpipeapi-createpipe
        </code></pre>
    2) 읽기, 쓰기 함수
        * 파이프 HANDLE을 입력하여 ReadFile, WriteFile 함수로 읽기와 쓰기를 한다.

### Named 파이프
1. 함수(Server)
    1) Named 파이프 생성 함수
        <pre><code>
        HANDLE CreateNamedPipeW(
            LPCSTR                lpName,                   // 파이프 이름
            DWORD                 dwOpenMode,               // 개방 모드(읽기, 쓰기, 읽기쓰기 모두)
            DWORD                 dwPipeMode,               // 데이터 전송 타입, 데이터 수신 타입, 블로킹 모드를 결정
            DWORD                 nMaxInstances,            // 생성 파이프 최대 개수(Client 연결 요청 수용 개수)
            DWORD                 nOutBufferSize,           // 출력 버퍼 사이즈(0: 디폴트)
            DWORD                 nInBufferSize,            // 입력 버퍼 사이즈(0: 디폴트)
            DWORD                 nDefaultTimeOut,          // WaitNamedPipe 함수에 적용할 기본 타임-아웃 시간(ms 단위)
            LPSECURITY_ATTRIBUTES lpSecurityAttributes      // 보안 속성
        );
        * 출처: https://docs.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-createnamedpipea
        </code></pre>
    2) 생성된 파이프를 연결 요청 대기 상태 변환
        <pre><code>
        BOOL ConnectNamedPipe(              // 설명: 클라이언트의 연결 요청을 수락, 연결 요청이 없는 상태라면 블로킹 됨
            HANDLE       hNamedPipe,        // CreateNamedPipeA에서 생성된 파이프 핸들
            LPOVERLAPPED lpOverlapped       // 중첩 I/O 관련 인자(미사용 시 NULL)
        );
        * 출처: https://docs.microsoft.com/en-us/windows/win32/api/namedpipeapi/nf-namedpipeapi-connectnamedpipe
        </code></pre>
2. 함수(Client)
    1) Named 파이프에 연결 요청 - CreateFile 함수
    2) 연결 요청 결과 대기
        <pre><code>
        BOOL WaitNamedPipeW(            
            LPCWSTR lpNamedPipeName,    // Named 파이프 이름(주소)
            DWORD   nTimeOut            // Server가 연결 요청을 수락할 때까지의 타임-아웃 시간 설정(ms 단위)
                                        // NMPWAIT_USE_DEFAULT_WAIT로 설정 시 CreateNamedPipeW 함수로 설정한 nDefaultTimeOut 시간으로 설정
        );
        * 출처: https://docs.microsoft.com/en-us/windows/win32/api/namedpipeapi/nf-namedpipeapi-waitnamedpipew
        </code></pre>
    3) 서버 파이프와의 연결 속성 변경(필요 시)
        <pre><code>        
        BOOL SetNamedPipeHandleState(
            HANDLE  hNamedPipe,             // 변경 시킬 파이프
            LPDWORD lpMode,                 // 읽기 모드와 함수 리턴방식
            LPDWORD lpMaxCollectionCount,   // 서버로 데이터를 보내기에 앞서 저장할 버퍼링 크기(Byte 단위)
            LPDWORD lpCollectDataTimeout    // 버퍼링 최대 허용 시간(ms 단위)
        );
        * 출처: https://docs.microsoft.com/en-us/windows/win32/api/namedpipeapi/nf-namedpipeapi-setnamedpipehandlestate
        </code></pre>
3. 통신 절차(Server = S, Client = C)
    1) S: Named 파이프 생성 (CreateNamedPipeW 함수)
    2) S: Named 파이프 연결 요청 대기 상태 변환(ConnectNamedPipe 함수)
    3) C: Named 파이프 연결 요청(CreateFile 함수)
    4) C: 연결 요청 결과 대기(WaitNamedPipeW)
    5) C: 파이프 연결 속성 변경(SetNamedPipeHandleState 함수), 이 과정은 필요 시
    6) S-C: 데이터 통신(ReadFile과 WriteFile )
    7) S: Pipe 연결 끊기(FulshFileBuffers 후 DisconnetNamedPipe 함수 호출)

## Section03 프로세스 환경변수
### 프로세스 환경변수
1. 환경변수: 프로세스별로 별도로 저장되는 key-value 문자열 구조의 메모리 공간
2. 부모는 자식 프로세스에게 환경변수를 **상속**시킬 수 있다.
3. 함수
    1) 환경변수 등록
        <pre><code>
        BOOL SetEnvironmentVariable(
            LPCTSTR lpName,     // 등록할 Key
            LPCTSTR lpValue     // Key에 해당하는 Value
        );
        * 출처: https://docs.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-setenvironmentvariable
        </code></pre>
    3) 환경변수 얻기
        <pre><code>
        DWORD GetEnvironmentVariable(
            LPCTSTR lpName,     // Key
            LPTSTR  lpBuffer,   // Key의 Value를 저장할 버퍼 주소
            DWORD   nSize       // lpBuffer가 가리키는 메모리 크기
        );
        * 반환: lpBuffer에 저장된 문자열 
        * 출처: https://docs.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-getenvironmentvariable
        </code></pre>

## Section04 명령 프롬프트 프로젝트 기능 추가
* 구현 내용: Chapter06에서 구현한 기능에 더하여 "현재 실행중인 프로세스 출력(lp)"과 "지정된 프로세스 제거(kp)" 명령을 추가한다.
* 코드 파일 경로: prj_prompt/chapter08
