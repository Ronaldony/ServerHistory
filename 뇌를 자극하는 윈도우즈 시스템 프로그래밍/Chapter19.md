# Chapter19 비동기 I/O와 APC
## Section01 비동기 I/O
### 비동기 I/O의 이해
1. 동기와 비동기 I/O
    1) 동기: I/O와 CPU연산이 독립적으로 차례차례 실행되는 방식이다.
        * 동기 I/O는 블로킹 함수를 사용한다.
    2) 비동기: I/O와 CPU연산이 병행되어 실행되는 방식이다.
        * 비동기 I/O는 넌블로킹 함수를 사용한다.
    * <img width=600 src="https://user-images.githubusercontent.com/95362065/150155790-d23b5084-8b9b-496f-befc-67df887fcca9.png">
2. 동기 I/O의 단점
    1) I/O 작업을 하는 동안 CPU가 하는 일이 없게 된다.
3. I/O라고 하면 네트워크 I/O, 파일 I/O, 모니터 I/O 등 여러가지 종류가 있다.

### 중첩 I/O
1. 
