# Chapter15 쓰레드 풀링
## Section01 쓰레드 풀에 대한 이해
### 쓰레드 풀
1. 쓰레드의 생성과 소멸은 시스템에 많은 부담을 준다. 쓰레드 풀은 이 같은 문제를 최소화하기 위하여 쓰레드를 소멸시키지 않고, 일정 공간에 저장해뒀다가 필요할 때 쓰레드를 다시 꺼내 쓰는 개념이다.
2. 쓰레드 풀의 기본적인 동작
    <pre><code>
    쓰레드 풀 생성: 쓰레드 대기열 생성
    -> 할 일 등록
    -> 쓰레드 배정 및 수행: 풀에서 할 일에 대한 쓰레드 할당 및 수행한다.  
    -> 쓰레드 회수: 할 일을 마친 쓰레드가 풀로 회수됨
    </code></pre>

## Section02 쓰레드 풀의 구현
### 쓰레드 풀 구현의 모듈별 해석
1. 쓰레드 풀 자료 구조
    <pre><code>
    // 쓰레드에게 시키는 일의 단위
    typedef void (*WORK)(void);
    
    // 쓰레드의 정보 구조체
    typedef struct __WorkerThread
    {
        HANDLE hThread;
        DWORD idThread;
    } WorkerThread;
    
    // Work와 Thread 관리를 위한 구조체, 쓰레드 풀
    struct __ThreadPool
    {
        // Work 등록 배열
        WORK workList[WORK_MAX];
        // Thread 정보와 각 Thread별 Event 오브젝트
        WorkerThread workerThreadList[THREAD_MAX];
        HANDLE workerEventList[THREAD_MAX];
        // Work에 대한 정보
        DWORD idxOfCurrentWork;     // 대기 1순위 Work Index
        DWORD idxOfLastAddedWork;   // 마지막 추가 Work Index + 1
        // 풀에 존재하는 쓰레드 개수
        DWORD threadIdx;        
    } gThreadPool;
    </code></pre>
2. 쓰레드 풀의 내부 구조
    1) Work List
        * <img width=650 src="https://user-images.githubusercontent.com/95362065/149902666-e8622d80-d99f-4f26-800c-01f04eccbb43.png">
    2) 쓰레드 List
        * workerThreadList: 풀에 저장된 쓰레드 정보가 담기는 배열 공간
        * workerEventList: 각 쓰레드별로 하나씩 할당되는 이벤트 동기화 오브젝트 배열 공간, workerThreadList의 배열 번호와 쌍을 이룬다.
        * threadIdx: 풀에 존재하는 쓰레드 개수
3. workerEventList가 필요한 이유
    1) 쓰레드에 할당된 일이 없을 땐 WaitFor~ 함수를 이용하여 Blocked 상태가 되어야 한다. 이후 일이 할당되면 다시 Running 상태가 되어야 하는데 이를 구현하기 위하여 Event 동기화 오브젝트가 필요하다.

### 쓰레드 풀의 함수 관계
1. 쓰레드 풀 함수
    <pre><code>
    // 쓰레드 풀에서 Work를 가져옴
    WORK GetWorkFromPool(void);
    
    // 쓰레드 풀에 새로운 Work를 등록
    DWORD AddWorkToPool(WORK work);
    
    // 쓰레드 풀에 쓰레드를 등록
    DWORD MakeThreadToPool(DWORD numOfThread);  // numOfThread: 이 수만큼 쓰레드가 생성됨
    
    // 쓰레드의 main 함수
    void WorkerThreadFunction(LPVOID pParam);   // pParam: 쓰레드의 번호(이벤트 동기화 오브젝트 핸들을 얻기 위함)
    </code></pre>
2. 쓰레드 풀 활용 절차
    <pre><code>
    Step1 쓰레드 등록(N개)
        MakeThreadToPool(N)
        
    Step2 Work 등록
        AddWorkToPool(work)
    
    Step3 풀에 등록된 Work 할당
        GetWorkFromPool(work)
        
    Step4 일이 할당된 쓰레드의 이벤트 동기화 오브젝트 Signeld 상태로 변경
    </code></pre>
