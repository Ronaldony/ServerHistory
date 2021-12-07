## section05 하드웨어 구성의 재접근
    1. CPU와 메모리 간 버스 시스템
        -CPU가 메모리로부터 명령어를 Fetch하는 과정에서 버스 시스템의 컨트롤 버스, 데이터 버스, 주소 버스가 하는 역할을 정리한다.
        -컨트롤 버스:
        -데이터 버스:
        -주소 버스:
    2. 고찰
        1) 윈도우에서 PC의 역할을 하는 레지스터는 IP이다.
            -CPU 내부에는 MAR(Memory Address Register), MBR(Memory Buffer Register)가 존재하여 Fetch 과정에 관여한다.
            -실제 Fetch 경로는 IP -> MAR -> 버스 시스템 경로를 거쳐 RAM으로부터 명령어를 전달 받음 -> MBR -> IR의 과정을 거친다.
        2) 실제 CPU와 RAM의 버스 시스템 경로(Intel)
            -CPU와 RAM 간 고속 전송을 위해 Front-Side-Bus(FSB) 혹은 System bus라 불리는 버스 시스템이 설계된다. FSB는 Level 2 Cache를 CPU와 RAM이 공유하게 된다.
            *참조: https://www.bbc.co.uk/bitesize/guides/zmb9mp3/revision/4, https://en.wikipedia.org/wiki/System_bus
        3) 마더보드의 속도는 버스 속도에 의해 정의된다. (TODO: 세부적인 내용 추가)
