# Chapter09 커널 오브젝트를 이용한 스레드 동기화
## 들어가기에 앞서
### 유저 모드 동기화
1. 최대 장점: 빠르다.
2. 한계점
    1) 크리티컬 섹션: **단일 프로세스 내**의 스레드들 사이에서만 동기화를 수행할 수 있음. 또한 대기 시간을 설정할 수 없어 데드락 상태에 빠지기 쉽다.

### 커널 모드 동기화
1. 유저->커널 모드로의 전환은 x86 플랫폼에서 약 200CPU 사이클 정도가 필요한 비싼 작업이다.

## Section01 대기 함수들
### WaitForSingleObject
1. WaitForSingleObject의 반환 값
    1) WAIT_OBJECT_0: 지정한 커널 오브젝트가 시그널 상태가 된 경우
    2) WAIT_TIMEOUT: 지정한 시간 값이 지나도 커널 오브젝트가 시그널 상태가 아닌 경우
    3) WAIT_FAILED: 유효하지 않은 커널 오브젝트를 전달한 경우, 이름있는 뮤텍스 미반환 등의 오류. 정확한 오류 코드는 GetLastError()를 통해 알 수 있다.

## Section02 성공적인 대기의 부가적인 영향
### Successful wait side effet
1. 한글로는 성공적인 대기의 부가적인 영향이라고 한다.
2. WaitForSingleObject와 WaitForMultipleObjects 함수는 오브젝트 생성 시 설정하는 자동 리셋 여부에 따라 '성공적인 호출' 시 오브젝트의 상태가 자동으로 변경되는 기능을 갖고 있다.
    * 성공적인 호출: 지정한 오브젝트(들)가 시그널 상태가 되어 반환되는 상황(WAIT_OBJECT_0)

### 원자적 동작의 순서
1. 원자적 동작
    1) 상황: 다른 두 스레드가 동시에 WaitForSingleObject를 호출한 상황, 지정한 오브젝트는 자동 리셋 이벤트 오브젝트이다.
    2) 원자적 동작: 오브젝트가 시그널 상태가 되면 하나의 스레드만이 스케줄상태가 되고 해당 오브젝트는 다시 논시그널 상태로 전환된다. 이때 나머지 스레드는 다시 오브젝트가 시그널 상태가 되기를 기다린다.
2. 오브젝트의 시그널 상태를 기다리는 두 개 이상의 스레드가 있을 때 어느 스레드를 깨울 것인가?
    * 본 책에서는 이렇게 말한다. MS의 공식적 답변은 "알고리즘은 공평하다"이며, 직접적인 동작 방식을 설명하지 않는다. 하지만 지금껏 MS의 알고리즘은 "선입선출" 방식을 따랐다고 언급하고 있다.

## Section03 이벤트 커널 오브젝트
### 이벤트 커널 오브젝트
1. 리셋 모드
    1) 자동 리셋: 오브젝트가 시그널 상태가 되면 이 이벤트를 기다리고 있던 스레드 중 하나만 스케줄 가능 상태가 된다. 이후 오브젝트는 논시그널 상태가 된다.
    2) 수동 리셋: 오브젝트가 시그널 상태가 되면 이 이벤트를 기다리고 있던 모든 스레드들은 동시에 스케줄 가능 상태가 된다.
2. 이벤트 오브젝트를 대기 중인 스레드의 동작
    1) 상황: 두 개 이상의 스레드가 논시그널 상태인 이벤트 오브젝트를 대기하던 중 시그널 상태가 되는 상황
    2) 동작: 멀티 CPU 환경이라면 모든 스레드가 각 CPU에서 동시에 수행된다.
    * 이는 본 책에서 언급하는 내용으로써 실제 동작은 확인 및 조사가 필요함

## Section04 대기 타이머 커널 오브젝트
### FILETIME과 LARGE_INTEGER
1. SetWaitableTimer 사용 시 LARGE_INTEGER* lpDueTime 인자 자리에 UTC로 정렬된 시간 값을 입력해야 한다. 이때 FILETIME 구조체 요소를 lpDueTime에 복사하여 시간 값으로 변환할 수 있다.
    * 이때 FILETIME 구조체의 주소는 32 비트 경계를 기준으로 정렬되어 있지만 LARGE_INTEGER는 64비트 기준이기 때문에 값을 복사하는 것이다. x86 프로세서의 경우 참조 비정렬 데이터 참조 문제를 내부적으로 처리하여 문제가 없지만, 다른 프로세서에 같은 코드로 빌드하면 데이터 정렬 에러(EXCEPTION_DATATYPE_MISALIGNMENT)가 발생한다.

### APC
1. APC 큐에 있는 APC 요청들이 모두 처리되어야 비로소 Alertable(알림 가능한) 함수가 반환된다.
    1) Alertable 함수: SleepEx, WaitForSingleObjectEx 등...

## Section07 편리한 스레드 동기화
* 이 섹션에는 오브젝트 논시그널 상태, 시그널 상태, Successful wait side effet에 관한 정리표가 작성되어 있다.

