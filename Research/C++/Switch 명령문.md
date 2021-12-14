# Switch 명령문
* 본 문서에서는 switch 문의 사용 방법에 따라 디스어셈블리의 결과를 비교 분석한다.
## Switch 문
### Case1 - case의 오름차순 정렬 (1씩 증가)
1. switch 내부 case 조건에 숫자를 1씩 증가하는 오름차순으로 정의한 경우의 디스어셈블리 결과를 관찰한다.
2. 디스어셈블리 결과
<pre><code>
/////////////////////// 코드
int input;
int output;

scanf("%d", &input);

switch (input)
00DF1091  mov         ecx,dword ptr [input]              
00DF1094  mov         dword ptr [ebp-0Ch],ecx  
00DF1097  mov         edx,dword ptr [ebp-0Ch]    
00DF109A  sub         edx,1                       ; edx = input - 1;
00DF109D  mov         dword ptr [ebp-0Ch],edx     
00DF10A0  cmp         dword ptr [ebp-0Ch],3       
00DF10A4  ja          $LN7+8h (0DF10D0h)          ; (input - 1) > 3이면 default 조건(0DF10D0h)으로 분기
00DF10A6  mov         eax,dword ptr [ebp-0Ch]     ; eax = input - 1;
00DF10A9  jmp         dword ptr [eax*4+0DF1104h]  ; [0DF1104h + (input - 1) * 4] 주소에 저장된 값으로 분기 (해당 번지 저장 값은 아래 명시)
{
case 1:         // 00DF10B0h
    output = input;
    break; 
case 2:         // 00DF10B8h
    output = input; 
    break; 
case 3:         // 00DF10C0h
    output = input;
    break;
case 4:         // 00DF10C8h
    output = input;
    break;
default:        // 00DF10D0h
    output = 0xFF;
    break;
}
  
/////////////////////// 메모리 저장 값
0x00DF1104  b0 10 df 00   // case 1 분기
0x00DF1108  b8 10 df 00   // case 2
0x00DF110C  c0 10 df 00   // case 3
0x00DF1110  c8 10 df 00   // case 4
</code></pre>
* 결과: case 개수만큼 오름차순  Jump 테이블을 생성한 후, switch(input)에서 input - 1을 Jump 테이블 인덱스로 사용하여 case x 번지로 분기한다. 

### Case2 - case의 내림차순 정렬
1. switch 내부 case 조건에 숫자를 1씩 감소하는 내림차순으로 정의한 경우의 디스어셈블리 결과를 관찰한다.
2. 디스어셈블리 결과
<pre><code>
/////////////////////// 코드
int input;
int output;

scanf("%d", &input);

switch (input)
007E1091  mov         ecx,dword ptr [input]  
007E1094  mov         dword ptr [ebp-0Ch],ecx  
007E1097  mov         edx,dword ptr [ebp-0Ch]  
007E109A  sub         edx,1                         ; edx = input - 1;
007E109D  mov         dword ptr [ebp-0Ch],edx
007E10A0  cmp         dword ptr [ebp-0Ch],3         ; 
007E10A4  ja          $LN7+8h (07E10D0h)            ; (input - 1) > 3이면 default 조건(07E10D0h)로 분기
007E10A6  mov         eax,dword ptr [ebp-0Ch]       ; eax = input - 1;
007E10A9  jmp         dword ptr [eax*4+7E1104h]     ; [7E1104h + (input - 1) * 4] 주소에 저장된 값으로 분기 (해당 번지 저장 값은 아래 명시)
{
case 4:     // 007E10B0
    output = input;
    break;  
case 3:     // 007E10B8
    output = input;
    break;
case 2:     // 007E10C0
    output = input;
    break;
case 1:     // 007E10C8
    output = input;
    break;
default:    // 007E10D0
    output = 0xFF;
    break;
}

/////////////////////// 메모리 저장 값
0x007E1104  c8 10 7e 00     // case 4 분기
0x007E1108  c0 10 7e 00     // case 3
0x007E110C  b8 10 7e 00     // case 2
0x007E1110  b0 10 7e 00     // case 1
</code></pre>
* 결과: Case1과 결과가 같다. 

### Case 3 - case의 임의 값 증가
1. switch 내부 case 조건에 숫자를 오름차순으로 하되 1 이상의 값을 증가시킨 경우의 디스어셈블리 결과를 관찰한다.
2. 디스어셈블리 결과
<pre><code>
/////////////////////// 코드
switch (input)
00EA1091  mov         ecx,dword ptr [input]  
00EA1094  mov         dword ptr [ebp-0Ch],ecx  
00EA1097  cmp         dword ptr [ebp-0Ch],3E8h      
00EA109E  jg          main+47h (0EA10B7h)       ; input > 1000 이면 0EA10B7로 분기
00EA10A0  cmp         dword ptr [ebp-0Ch],3E8h  
00EA10A7  je          main+62h (0EA10D2h)       ; input == 1000 이면 case 1000(0EA10D2h)으로 분기
00EA10A9  cmp         dword ptr [ebp-0Ch],32h  
00EA10AD  je          main+52h (0EA10C2h)       ; input == 50 이면 case 50(0EA10C2h)으로 분기
00EA10AF  cmp         dword ptr [ebp-0Ch],64h  
00EA10B3  je          main+5Ah (0EA10CAh)       ; input == 100 이면 case 100(0EA10CAh)으로 분기
00EA10B5  jmp         main+72h (0EA10E2h)       ; default로 분기
00EA10B7  cmp         dword ptr [ebp-0Ch],7D0h  
00EA10BE  je          main+6Ah (0EA10DAh)       ; input == 2000 이면 case 2000(0EA10DAh)으로 분기
00EA10C0  jmp         main+72h (0EA10E2h)       ; default로 분기
{
case 50:        // 00EA10C2
    output = input; 
    break;
case 100:       // 00EA10CA
    output = input;
    break;
case 1000:      // 00EA10D2
    output = input;
    break;
case 2000:      // 00EA10DA
    output = input;
    break;
default:        // 00EA10E2
    output = 0xFF;
    break;
}
</code></pre>
* 결과: Jump 테이블이 생성되지 않고 input 값을 일일히 비교하여 해당 case로 분기한다.
### Case 4 - case의 무작위 정렬
<pre><code>	
/////////////////////// 코드
int input;
int output;

scanf("%d", &input);

switch (input)
00271091  mov         ecx,dword ptr [input]  
00271094  mov         dword ptr [ebp-10h],ecx  
00271097  mov         edx,dword ptr [ebp-10h]  
0027109A  sub         edx,1                         ; edx = input - 1;
0027109D  mov         dword ptr [ebp-10h],edx  
002710A0  cmp         dword ptr [ebp-10h],0Eh  
002710A4  ja          $LN9 (02710DFh)               ; (input - 1) > 14이면 default 조건(02710DFh)로 분기
002710A6  mov         eax,dword ptr [ebp-10h]  
002710A9  movzx       ecx,byte ptr [eax+271128h]    ; ecx = [271128h + (input - 1)](단, 1byte만큼 접근) => [addr]: addr 번지에 저장된 값
                                                    ; 271128h 번지에 저장된 데이터 아래 명시
002710B0  jmp         dword ptr [ecx*4+271110h]     ; [271110h + ecx * 4]로 분기
                                                    ; 271110h 번지에 저장된 데이터 아래 명시
{
case 10:        // 002710B7h
    output = input;
    break;
case 5:         // 002710BFh
    output = input;
    break; 
case 1:         // 002710C7h
    output = input;
    break;
case 6:         // 002710CFh
    output = input;
    break;
case 15:        // 002710D7h
    output = input;
    break;
default:        // 002710DFh
    output = input;
    break;
}
/////////////////////// 메모리 저장 값
// 271128h
0x00271128  00 05 05 05     ; 0: case 1 분기에 대한 Index (271110h 메모리 저장 값과 매칭)
0x0027112C  01 02 05 05     ; 1: case 5 Index, 2: case 6 Index
0x00271130  05 03 05 05     ; 3: case 10 Index
0x00271134  05 05 04 3b     ; 4: case 15 Index
// 271110h
0x00271110  c7 10 27 00     ; case 1 번지 
0x00271114  bf 10 27 00     ; case 5 번지
0x00271118  cf 10 27 00     ; case 6 번지
0x0027111C  b7 10 27 00     ; case 10 번지
0x00271120  d7 10 27 00     ; case 15 번지
0x00271124  df 10 27 00     ; default 번지
</code></pre>
* 결과: 


## if 문
### 
