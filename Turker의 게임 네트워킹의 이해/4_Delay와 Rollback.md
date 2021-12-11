# Delay와 Rollback
## Deterministic(3_Deterministic 이어서)
1. 네트워크 Latency에 대한 처리가 잘 되지 않으면 Desync 상태가 된다.
2. 네트워크 Latency에 따른 Deterministc 처리 과정
  1) 각 유저가 키 입력 시 입력에 대한 처리를 네트워크 Latency만큼 기다린 후 이전에 입력된 타 유저의 입력에 따라 결과를 계산한다.

## Delay
1. Deterministic 방식의 처리로 인하여 유저가 느끼는 결과 도출에 대한 지연을 의미한다.
2. 100ms 지연은 일반 유저의 경우 지연되는 것을 느끼지 못하지만 200ms의 경우 먹먹한 느낌을 받는다.
  * 보통 100ms 지연이 Maximum이라고 본다.

## Rollback
1. Delay를 해결하기위한 방법론
2. 
