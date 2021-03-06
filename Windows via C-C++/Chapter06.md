# Chapter06 스레드의 기본
## Section01 스레드를 생성해야 하는 경우
### 스레드의 두 요소
1. 두 요소
    1) 스레드 커널 오브젝트: 시스템이 스레드에 대한 통계 정보를 저장하는 공간
    2) 스레드 스택: 함수의 매개변수와 지역변수 저장 공간

### 스레드
1. 프로세스의 초기화가 진행되는 동안에 시스템은 주 스레드를 생성한다.
    * 응용 프로그램이 MS C/C++ 컴파일러로 작성된 경우라면 주 스레드는 C/C++ 런타임 라이브러리의 시작 코드를 수행하는 것으로 시작된다. 이후 main 함수로 진입하여 함수가 반환되면 C/C++ 런타임 라이브러리의 시작 코드가 ExitProcess를 호출하여 수행을 종료한다.
2. CPU는 놀면 안 된다. 따라서 Windows는 낮은 우선순위 스레드를 사용하영 다음과 같은 작업을 한다.
    1) 인덱스 서비스(윈도우->검색 기능)
    2) 디스크 조각 모음
    3) Visual Studio에서 사용자가 코드 작성이 멈추면 코드를 자동 컴파일하는 기능. 이 기능으로 사용자는 미리 잘못된 부분에 밑줄이나 경고 및 에러를 발견한다.
    4) Word(문서 작업 프로그램)에서의 페이지 번호 재계산, 철자와 문법 검사, 프린팅 등..
    5) 다른 미디어로의 파일 복사 같은 작업
3. 위에서 언급한 동작들은 거의 백그라운드에서 동작하는데, 이는 잡스러운 기능들을 백그라운드 기능으로 넘기면서 사용자 인터페이스를 좀 더 단순화하는 효과가 있다. 때문에 멀티스레드를 이용하면 좀 더 확장성이 좋은 구조로 설계할 수 있다.

## Section02 스레드를 생성하지 말아야 하는 경우
### 윈도우 어플리케이션
1. 대부분 어플리케이션에서 사용자 인터페이스를 위한 컴포넌트들은 반드시 **하나의 스레드**를 사용해야만 한다.
    * 하나의 스레드를 사용하여 모든 윈도우의 차일드 윈도우를 생성하는 것이 좋다! => why?
2. 윈도우 어플리케이션은 보통 윈도우를 생성하고 GetMessage 루프를 가진 **단일 사용자 인터페이스 스레드**와 그 외 계산이나 IO 위주의 작업을 수행하는 **워커 스레드**로 동작한다.
    * 보통 우선순위를 단일 사용자 인터페이스 스레드 > 워커 스레드 와 같이 부여하여 사용자 응답성을 개선할 수 있다.
    * 다수 사용자 인터페이스 스레드가 유용한 경우: 윈도우 탐색기

## Section03 처음으로 작성하는 스레드 함수
### 스레드 함수 사용 시 중요한 점
1. 스레드 함수 이름은 각기 다른 이름으로 구현될 수 있다.
2. 스레드 함수에는 하나의 매개변수만 전달 할 수 있다.
3. 스레드 함수는 반드시 값을 반환해야 한다. 이는 쓰레드 커널 오브젝트에 저장되는 종료코드를 의미하기 때문이다.
4. 스레드 함수는 가능한 한 함수로 전달된 매개변수와 지역 변수만을 사용하도록 작성되는 것이 좋다.

## Section04 CreateThread 함수
### CreateThread
1. CreateThread가 호출되면 시스템은 스레드 커널 오브젝트를 생성하고 스레드가 사용할 스택을 확보한다.
2. CreateThread 구조
    <pre><code>
    
    </code></pre>
