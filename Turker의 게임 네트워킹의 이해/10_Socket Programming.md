# 10화 Socket Programming
## MMORPG
### OS
1. HAL(Hardware Abstraction Layer): 컴퓨터의 물리적인 하드웨어와 컴퓨터에서 실행되는 소프트웨어 사이의 추상화 계층이다.
    * 개발자는 HAL을 통해 LAN 카드의 기능을 소프트웨어적으로 사용할 수 있다.

### 소켓 통신 프로그램
1. 비동기 I/O 통신
    1) 대표적으로 윈도우즈에는 select와 IOCP 방식이 있다.
    2) select: pre request 방식
    3) IOCP: post request 방식, 대응되는 방식으로 리눅스에는 epoll 방식이 있다.
