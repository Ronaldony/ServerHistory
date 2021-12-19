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
1. 
