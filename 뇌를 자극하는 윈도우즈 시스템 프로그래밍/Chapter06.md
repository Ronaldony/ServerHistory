# Chapter06 커널 오브젝트와 오브젝트 핸들
## Section01 커널 오브젝트에 대한 이해
### 커널 오브젝트의 이해
1. 커널 오브젝트: 커널에서 관리하는 중요한 정보를 담아둔 데이터 블록
2. CreateProcess 함수 호출 시 프로세스를 생성하는 주체는 사실 **OS**이다. 프로그래머가 함수를 호출하여 요청하는 것일뿐, 프로세스의 **생성과 관리**를 주체하는 것은 OS이다.
    1) 프로세스를 관리하기 위해서는 프로세스 상태 정보, 우선순위와 같은 정보를 관리해야 한다. 이러한 정보들의 데이터 구조체 덩어리를 **커널 오브젝트**라 한다. 이 커널 오브젝트는 OS 내부에 존재한다.
3. 커널 오브젝트는 프로그래머가 **직접 조작**할 수 없다.

### 그 이외의 커널 오브젝트들
1. 프로세스 이 외에도 쓰레드, IPC(Inter Process Communication), 파일 등의 리소스를 생성할 때에도 커널 오브젝트를 생성해서 필요한 정보들을 저장한다.
2. 생성되는 커널 오브젝트의 종류에 따라서 서로 다른 구조체를 기반으로 생성된다.

### 오브젝트 핸들(Handle)을 이용한 커널 오브젝트의 조작
1. 마이크로소프트에서 제공해주는 시스템 함수를 통해 프로그래머는 커널 오브젝트를 **간접적으로 조작** 할 수 있다.
2. 핸들 or 오브젝트 핸들: 커널 오브젝트 생성 시 프로그래머에게 부여되는 커널 오브젝트 할당 정수값 번호. 이 번호로 원하는 커널 오브젝트에 접근하여 간접적 조작을 할 수 있다.
    * 핸들-오브젝트-리소스의 관계
    <img width="500" src="https://user-images.githubusercontent.com/95362065/146154142-07d0d53d-c339-44f8-8b75-6766385032ec.png">
3. 핸들을 사용하여 프로세스의 우선순위를 변경시키는 함수
    1) BOOL SetPriorityClass(HANDLE, DWORD)

### CPU 점유권
1. 함수가 호출되는 중간에도 CPU의 점유권은 다른 프로세스에게 넘어갈 수 있다.

## Section02 커널 오브젝트와 핸들의 종속 
### 커널 오브젝트의 종속 관계
1. 커널 오브젝트는 Windows 운영체제(OS)에 종속적이다.
    * 본 책에서는 커널 오브젝트를 도서 대여점(OS)이 고객(프로세스)에게 빌려주는 책에 비유한다. 고객은 책을 "빌려" 볼 뿐이고 책은 도서 대여점에 "종속"적이다.
    * 위 관계에 더하여 커널 오브젝트의 소멸 시점은 운영체제에 의해 결정된다.
2. 커널 오브젝트가 OS에 종속적인 이유
    1) 커널 오브젝트의 소멸시점은 OS에 의해 결정된다.
    2) 하나의 커널 오브젝트는 하나의 프로세스가 아닌 여러 프로세스에 의해 접근될 수 있다. (예를 들어 프로세스 생성 시 프로세스 오브젝트에 부모와 자식 프로세스가 접근하고 있는 경우)

### 핸들의 종속 관계
1. 핸들은 OS가 아닌 **프로세스**에 종속적이다. 이에 대한 내용은 Chapter08에서 자세히 다룬다.

### 에제를 통한 종속 관계 이해 (예제1)
* 본 책의 예제를 일부 변경하여 커널 오브젝트 접근에 관한 내용을 설명하고자 하였다. (코드 파일 경로: Chapter06_src/예제1)
* 본 예제를 통하여 커널 오브젝트가 프로세스가 아닌 OS에 종속적인 관계라는 것을 알 수 있다.

### PROCESS_INFORMATION 구조체
1. 구조체 구성
<pre><code>
typedef struct _PROCESS_INFORMATION {
  HANDLE hProcess;          // 프로세스 핸들(커널 오브젝트를 구분하기 위한 값)
  HANDLE hThread;           // 쓰레드 핸들
  DWORD  dwProcessId;       // 프로세스 ID (프로세스 자체를 구분하기 위한 값)
  DWORD  dwThreadId;        // 쓰레드 ID
} PROCESS_INFORMATION, *PPROCESS_INFORMATION, *LPPROCESS_INFORMATION;
</code></pre>
 * 출처: https://docs.microsoft.com/en-us/windows/win32/api/processthreadsapi/ns-processthreadsapi-process_information

## Section03 커널 오브젝트와 Usage Count
* 커널 오브젝트의 생성 주체는 프로세스가 아닌 "OS"이며 소멸주기 또한 생성의 주체인 "OS"가 결정한다.
### CloseHandle 함수에 대한 정확한 이해
1. 프로세스가 소멸된다고 해서 커널 오브젝트가 소멸되지는 않는다. 
2. CloseHandle 함수 
    1) BOOL CloseHandle(HANDLE): HANDLE에 해당하는 커널 오브젝트의 Usage Count를 -1 시키고, 핸들 테이블에서 해당 핸들 번호를 삭제한다.
    2) 오해: CloseHandle은 리소스와 커널 오브젝트를 소멸시키는 것이 아니다!

### CloseHanadle 함수와 프로세스 종료코드 
1. 종료코드: 프로세스가 종료되며 반환하는 값. 종료 코드를 반환하는 기능엔 exit(), ExitProcess(), 메인 함수에서의 return 등이 있다. 
2. 프로세스가 종료되면 종료코드는 종료된 프로세스의 커널 오브젝트에 저장된다.
3. 종료코드 관련 함수
    1) BOOL GetExitCodeProcess(HANDLE, LPDWORD): HANDLE에 해당하는 프로세스의 **종료코드**를 LPDWORD로 반환받는다.
        * 해당 함수는 즉시 반환하며(넌블로킹), HANDLE에 해당하는 프로세스가 종료되지 않은 경우 STILL_ALIVE를 종료코드로 반환한다.
    2) exit(int const): 프로세스를 강제로 종료시킨다. int const에 입력한 값으로 **종료코드**가 프로세스 오브젝트에 저장된다.

### 커널 오브젝트와 Usage Count
1. Windows는 커널 오브젝트를 참조하는 대상이 하나도 없을 때 소멸시기로 정한다. 이 참조하는 대상을 관리하기 위해 **Usage count**라는 것을 관리한다. 이 Usage count가 0이 되는 순간 커널 오브젝트는 소멸된다
2. 프로세스는 생성과 동시에 Usage count가 1이 된다. 만약 0으로 생성되면 생성과 동시에 소멸되기 때문이다. 
3. Usage count의 증가는 커널 오브젝트에 접근 가능한 핸들 개수의 증가를 의미한다. 반대로 감소는 핸들 개수의 감소를 의미한다.
4. 자식 프로세스의 Usage count
    * <img width="500" src="https://user-images.githubusercontent.com/95362065/146329864-936b749f-0119-47cd-a80d-f0fe91f02cda.png">
    * 일반적으로 부모프로세스는 자식 프로세스를 생성한 뒤 CloseHandle 함수를 호출하여 자식 프로세스의 핸들 번호를 반환한다.

## Section04 명령 프롬프트 프로젝트 기능 추가
* 구현 내용: Chapter05에서 구현한 기능에 더하여 "start", "echo"명령을 추가한다.
* 코드 파일 경로: prj_prompt/chapter06
