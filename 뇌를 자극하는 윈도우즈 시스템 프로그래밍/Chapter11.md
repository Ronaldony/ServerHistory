# Chapter 11 쓰레드의 이해
## Section01 쓰레드란 무엇인가?
### 멀티 프로세스 운영체제 기반 프로그램의 문제점과 새로운 제안
1. 프로세스 상태 정보르 저장하고 복원하는 **컨텍스트 스위칭**은 성능 저하의 원인이 된다.
2. 쓰레드 탄생 배경: 컨텍스트 스위칭으로 인한 성능 저하를 줄이자! => 저장 및 복원하는 컨텍스트 정보 개수를 줄이자!

### 해결책, 쓰레드
1. 쓰레드는 하나의 프로그램에서 두 개 이상으 실행 흐름을 갖게 할 수 있다.
2. 프로세스와 달리 쓰레드간에는 공유하는 상태 정보들이 있다. 이것이 쓰레드 간 컨텍스트 스위칭이 프로세스보다 빠른 이유이다.

### 메모리 구조 관점에서 본 프로세스와 쓰레드
1. 멀티 프로세스와 멀티 쓰레드 
    1) 멀티 프로세스
        * 서로 다른 프로세스는 서로의 메모리 공간(Data, Code, Heap, Stack Section)을 공유하지 않는다. 따라서, IPC를 사용하여 통신해야 한다.
        * 구조 그림 추가
    2) 멀티 쓰레드
        * 같은 프로세스 내 존재하는 쓰레드 간에는 Stack을 제외한 Data, Code, Heap Section을 공유한다.
        * 구조 그림 추가
2. 쓰레드의 특성
    1) 쓰레드마다 Stack을 독립적으로 할당해 준다. 실행 흐름의 추가를 위한 최소 조건이 독립된 스택의 제공인 것이다.
    2) **코드 영역**을 공유한다. 이에 따라 모든 쓰레드는 같은 전역 함수, 클래스 등에 접근할 수 있다.
    3) **데이텨, 힙 영역**을 공유한다. IPC가 필요 없다. 하지만, 메모리 공유 시 동기화 등의 문제가 발생한다.

### Windows에서의 프로세스와 쓰레드
1. Windows 입장에서 프로세스는 단순히 **쓰레드를 담는 상자**에 지나지 않는다.
2. 속도가 멀티 프로세스 < 멀티 쓰레드인 이유
    1) 컨텍스트 스위칭 정보 감소
    2) 캐시 메모리 내 데이터 사용 유지
3. 사실 프로세스는 상태(Running, Ready, Blocked)를 지니지 않는다. 상태를 갖는 주체는 쓰레드이다.
    * 따라서, 스케줄러는 쓰레드를 실행 단위로 스케줄링한다.
4. 프로세스 생성 시 반드시 **main 쓰레드**가 생성된다.

## Section02 쓰레드 구현 모델에 따른 구분
### 커널 레벨(Kernel Level) 쓰레드와 유저 레벨(User Level)쓰레드
1. 쓰레드의 생성
    1) 커널 레벨 쓰레드
        * 시스템 함수 호출에 의해 생성되며 OS가 해당 쓰레드를 생성 및 관리한다.
        * 커널 레벨의 스케줄러가 스케줄링을 한다.
        * 스케줄러와 쓰레드 정보(스케줄링 시 필요한 쓰레드 정보)는 커널 영역에, 프로그램 코드는 유저 영역에 존재한다.
        * 장점: 안정성, 다양한 기능성을 제공한다.
        * 단점: 커널 모드로의 전환이 빈번하게 일어나 성능 저하로 이어진다
    2) 유저 레벨 쓰레드
        * 커널에 의존적이지 않은 쓰레드 기능의 라이브러리에 의해 쓰레드의 생성 및 관리된다.
        * 유저 영역에서 쓰레드 정보 저장과 스케줄링이 일어난다. 커널 영역의 스케줄러는 프로세스를 기준으로 스케줄링하게 된다.
        * 운영체제는 쓰레드의 존재 여부를 알지 못한다.
        * 장점: 성능이 좋다.
        * 단점: 쓰레드가 여러 개일 때 시스템 함수로 인하여 블로킹이 걸리면 모든 쓰레드가 블로킹 상태가 된다. 해결 방법이 있지만 결과 예측이 어렵다.
2. 유저, 커널 영역
    1) 유저 영역
        * 하나의 프로세스에 할당된 Code, Data, Heap, Stack 영역을 말한다.
    2) 커널 영역
        * 유저 영역을 제외한 나머지 영역을 의미한다. 
        * 32bit(최대 4GB), 64bit(최대 16TB) OS에서는 RAM의 1/2은 유저, 1/2은 커널 영역을 나뉜다.

### 커널 모드와 유저 모드
1. Windows는 동작할 때 커널 모드와 유저 모드 중 한가지 모드로 동작한다.
    1) 커널 모드
        * Windows 커널이 실행되어야 하는 경우에는 커널 모들로의 전환이 일어난다. (예를 들어 타임 슬라이스마다 스케줄러 동작으로 인한 커널 모드 전환이 일어남)
        * 유저 영역으로의 접근이 가능
        * Windows에서 제공하는 시스템 함수들 중 일부는 커널 모드로의 전환이 필요하다.
    2) 유저 모드
        * 일반적인 프로그램은 유저 모드에서 동작한다.
        * 커널 영역으로의 접근이 불가능 (접근 시도 시 시스템에서 오류 발생시킴)
    * 커널 모드와 유저 모드를 제공하는 것은 운영체제가 아닌 **프로세서**이다!!!!! 메모리 보호 기능이 CPU에 달려있는 것이다.
