# 11화 MMORPG 서버 구조(1/3)
## MMORPG
### 서버 특징
1. 작업량이 많다. 게임에서는 Actor 단위라 부른다.
    1) Actor: 작업의 모든 요소, 예를 들어 Player, NPC, 몬스터, ControlZone 등..
    * ControlZone: 제어되는 특정 영역, 예를 들면 플레이어가 바닷가에서 잠수를 하면 산소 게이지가 나타나는데 이러한 특정 상황을 제어하는 영역을 뜻한다.

### 서버가 하는 일
1. 서버가 하는 일
    1) 패킷 처리(Read/Write): 클라-서버 간 통신하는 메시지 처리
    2) Tick 처리: 매 프레임마다 시간을 기준으로 Actor의 정보를 업데이트하는 작업
    3) 그 외 System: 경매장 거래 시스템, 공성전 시스템 등..
2. 패킷 처리와 Tick 처리가 작업량의 대부분을 차지한다.(흔히 80대 20법칙이라고 불림)

### 서버의 스레드
1. 멀티 스레드에서는 잘 짜여진 구조가 필수적이다. 만약 구조가 이상하다면 오히려 싱글 스레드보다 더 효율이 안 좋을수 있다.
2. 싱글 스레드의 구조
    1) Network I/O Thread: select, IOCP 등을 이용한 네트워크
    2) Work Thread: 매 Tick마다 Actor에 대한 처리 및 Network I/O로부터 전달받은 데이터를 처리
    3) 단점: 처리할 수 있는 Actor 수가 적다. 대개 1~2천 명을 커버하고 잘 만들면 3~4천 명이다.
    * 보통 Network I/O와 Work Thread 사이에 큐를 생성하여 데이터를 주고 받는다.
3. 싱글 스레드 + Alpha(Dedicate Thread) 구조
    1) 이 구조에서 Dedicated Thread의 역할은 정해진 일만을 하게 된다. 예를 들어 경매장, 공성전 등..
4. 멀티 스레드의 구조
    1) 하나의 Network I/O Thread와 다수의 Work Thread로 구성
    2) 단점을 극복하기 위해 제대로 된 구획화가 필요하다.
    3) 단점: 동기화 문제를 제대로 해결하지 않으면 데드락이 발생한다. 이에 따라 구현이 어렵다.
    * SeamLess 서버의 경우 멀티 스레드가 필수적이다.

## 기타 키워드
### Zoom, Room, SeamLess 서버
1. Zoom: 이동 가능한 지역을 로드하는 과정(맵과 맵 사이)의 부분 로딩을 말한다.(마영전, 던파 등..)
2. Room: 처음 시작 시 모든 데이터를 로드하는 방식을 말한다.(롤, 카트, 오버워치 등..)
3. SeamLess: 현재 위치를 예측해서 모든 맵이 이동 없이 로드되는 방식을 말한다. (검은 사막, 마인크래프트, GTA 등..)
    * 오픈 월드의 느낌

# 12화 MMOPRG서버구조(2/3)
## MMORPG
### 싱글 프로세스와 멀티 프로세스 out
1. 이 영상에서는 성능 향상 방법에 싱글 프로세스는 Scale up, 멀티 프로세스는 Scale out 방식이 있다고 설명한다.
    * 멀티 프로세스에서는 하나의 프로세스를 서버 중 하나의 채널로 본다. 채널들의 유저수, 채널 간 이동 등의 기능을 관리하는 서버가 하나 설계되어야 한다.
2. Single point of fail
    1) 싱글 프로세스에서 발생하는 문제로 해당 프로세스가 죽으면 서비스 전체(서버)가 마비되는 상황을 의미한다.
        * 반면, 멀티 프로세스의 경우 나머지 프로세스들이 서비스를 지속하기 때문에 가용성이 높다고 표현한다.
3. 싱글과 멀티 프로세스 장단점
    1) 싱글: 가용성이 낮고, 비용이 높음
    2) 멀티: 가용성이 높고, 비용이 낮음

# 13화 MMOPRG서버구조(3/3)
## 멀티 스레드
### 충돌 처리
1. AKKA 방식: 보통 두 Actor A, B가 있을 때 A가 B에 공격을 가하는 것은 B가 가진 메시지 버퍼에 메시지를 넣은 후 B가 처리하는 방식으로 구현된다. 
    1) 문제 1: A가 B의 상태를 조회해야하는 경우에 동기화 문제가 발생한다.(다른 Actor도 공격할 수 있기 때문)  예를 들어 HP의 퍼센트 비율로 데미지가 들어가는 공격인 경우이다.
    2) 문제 2: 비동기적으로 처리 시 다수의 Actor 간의 
2. ECS(Entity Component System) 구조
    1) Entity: Actor와 유사한 의미이며, 단순히 Component의 묶음이다.
    2) Component: Class가 아닌 Data의 묶음이라고 볼 수 있다. 예를 들어 Flying Component는 기능에 필요한 스피드, 고도 등의 데이터를 말한다.
    3) System: Data가 없고 기능만을 담당하며, 기능별로 Entity, Component와의 관계가 달라진다.
    4) 장점
        * System 기준으로 스레드를 나눌 수 있다.
        * 확장성이 좋다. Entity이던 소유한 Component에 따라 성격이 달라질 수 있다.
        * System별로 사용되는 데이터가 Entity로 묶여 있기 때문에 데이터 로컬리티가 좋다. 
    5) 해결해야하는 점: System 간의 인터렉션이 있을 수 있고 그로 인해 System과 Component가 1:N의 관계가 될 수 있다. 이로 인해 Component 의존 관계가 생겨나고 동기화 문제가 발생된다. 즉, AKKA 방식에서 발생한 문제가 똑같이 일어남
    * MVC 모델의 게임 버전이다.
