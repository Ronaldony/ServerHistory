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
00DF1097  mov         edx,dword ptr [ebp-0Ch]     ; edx = input; (input 값)
00DF109A  sub         edx,1                       ; edx--;
00DF109D  mov         dword ptr [ebp-0Ch],edx     
00DF10A0  cmp         dword ptr [ebp-0Ch],3       ; (input - 1) > 3이면 
00DF10A4  ja          $LN7+8h (0DF10D0h)          ; 0DF10D0h로 분기 (0DF10D0h은 default 조건)
00DF10A6  mov         eax,dword ptr [ebp-0Ch]     ; eax = input - 1;
00DF10A9  jmp         dword ptr [eax*4+0DF1104h]  ; [0DF1104h + (input - 1) * 4] 주소에 저장된 값으로 분기 (해당 번지 저장 값은 아래 명시)
{
case 1:
    output = input;
00DF10B0  mov         ecx,dword ptr [input]  
00DF10B3  mov         dword ptr [output],ecx  
    break;
00DF10B6  jmp         $LN7+0Fh (0DF10D7h)  
case 2:
    output = input;
00DF10B8  mov         edx,dword ptr [input]  
00DF10BB  mov         dword ptr [output],edx  
    break;
00DF10BE  jmp         $LN7+0Fh (0DF10D7h)  
case 3:
    output = input;
00DF10C0  mov         eax,dword ptr [input]  
00DF10C3  mov         dword ptr [output],eax  
    break;
00DF10C6  jmp         $LN7+0Fh (0DF10D7h)  
case 4:
    output = input;
00DF10C8  mov         ecx,dword ptr [input]  
00DF10CB  mov         dword ptr [output],ecx  
    break;
00DF10CE  jmp         $LN7+0Fh (0DF10D7h)  
default:
    output = 0xFF;
00DF10D0  mov         dword ptr [output],0FFh  
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
007E10A0  cmp         dword ptr [ebp-0Ch],3         ; (input - 1) > 3이면 
007E10A4  ja          $LN7+8h (07E10D0h)            ; 07E10D0h로 분기(default 조건)
007E10A6  mov         eax,dword ptr [ebp-0Ch]       ; eax = input - 1;
007E10A9  jmp         dword ptr [eax*4+7E1104h]     ; [7E1104h + (input - 1) * 4] 주소에 저장된 값으로 분기 (해당 번지 저장 값은 아래 명시)
{
case 4:
    output = input;
007E10B0  mov         ecx,dword ptr [input]  
007E10B3  mov         dword ptr [output],ecx  
    break;
007E10B6  jmp         $LN7+0Fh (07E10D7h)  
case 3:
    output = input;
007E10B8  mov         edx,dword ptr [input]  
007E10BB  mov         dword ptr [output],edx  
    break;
007E10BE  jmp         $LN7+0Fh (07E10D7h)  
case 2:
    output = input;
007E10C0  mov         eax,dword ptr [input]  
007E10C3  mov         dword ptr [output],eax  
    break;
007E10C6  jmp         $LN7+0Fh (07E10D7h)  
case 1:
    output = input;
007E10C8  mov         ecx,dword ptr [input]  
007E10CB  mov         dword ptr [output],ecx  
    break;
007E10CE  jmp         $LN7+0Fh (07E10D7h)  
default:
    output = 0xFF;
007E10D0  mov         dword ptr [output],0FFh  
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
00EA109E  jg          main+47h (0EA10B7h)       ; input > 0x3E8 이면 0EA10B7로 분기
00EA10A0  cmp         dword ptr [ebp-0Ch],3E8h  
00EA10A7  je          main+62h (0EA10D2h)       ; input == 0x3E8 이면 case 1000(0EA10D2h)으로 분기
00EA10A9  cmp         dword ptr [ebp-0Ch],32h  
00EA10AD  je          main+52h (0EA10C2h)       ; input == 0x32 이면 case 50(0EA10C2h)으로 분기
00EA10AF  cmp         dword ptr [ebp-0Ch],64h  
00EA10B3  je          main+5Ah (0EA10CAh)       ; input == 0x64 이면 case 100(0EA10CAh)으로 분기
00EA10B5  jmp         main+72h (0EA10E2h)       ; default로 분기
00EA10B7  cmp         dword ptr [ebp-0Ch],7D0h  
00EA10BE  je          main+6Ah (0EA10DAh)       ; input == 0x7D0h 이면 case 2000(0EA10DAh)으로 분기
00EA10C0  jmp         main+72h (0EA10E2h)       ; default로 분기
{
case 50:
    output = input;
00EA10C2  mov         edx,dword ptr [input]  
00EA10C5  mov         dword ptr [output],edx  
    break;
00EA10C8  jmp         main+79h (0EA10E9h)  
case 100:
    output = input;
00EA10CA  mov         eax,dword ptr [input]  
00EA10CD  mov         dword ptr [output],eax  
    break;
00EA10D0  jmp         main+79h (0EA10E9h)  
case 1000:
    output = input;
00EA10D2  mov         ecx,dword ptr [input]  
00EA10D5  mov         dword ptr [output],ecx  
    break;
00EA10D8  jmp         main+79h (0EA10E9h)  
case 2000:
    output = input;
00EA10DA  mov         edx,dword ptr [input]  
00EA10DD  mov         dword ptr [output],edx  
    break;
00EA10E0  jmp         main+79h (0EA10E9h)  
default:
    output = 0xFF;
00EA10E2  mov         dword ptr [output],0FFh  
    break;
}
</code></pre>
* 결과: Jump 테이블이 생성되지 않고 input 값을 일일히 비교하여 해당 case로 분기한다.
### Case 4 - case의 무작위 정렬
<pre><code>	
switch (input)
00B71091  mov         ecx,dword ptr [input]  
00B71094  mov         dword ptr [ebp-0Ch],ecx  
00B71097  cmp         dword ptr [ebp-0Ch],64h  
00B7109B  jg          main+57h (0B710C7h)       ; input > 100 이면 0B710C7h로 분기
00B7109D  cmp         dword ptr [ebp-0Ch],64h  
00B710A1  je          main+0A0h (0B71110h)      ; input == 100 이면 case 100(0B71110h)으로 분기
00B710A3  mov         edx,dword ptr [ebp-0Ch]  
00B710A6  sub         edx,4                     ; edx = input - 4;
00B710A9  mov         dword ptr [ebp-0Ch],edx  
00B710AC  cmp         dword ptr [ebp-0Ch],2Eh  
00B710B0  ja          $LN13 (0B71140h)          ; (input - 4) > 46 이면 default 조건(0B71140h)으로 분기
00B710B6  mov         eax,dword ptr [ebp-0Ch]  
00B710B9  movzx       ecx,byte ptr [eax+0B71188h]  
00B710C0  jmp         dword ptr [ecx*4+0B71174h]  
00B710C7  cmp         dword ptr [ebp-0Ch],3E8h  
00B710CE  jg          main+7Dh (0B710EDh)       ; (input - 4) > 1000 이면 0B710EDh로 분기
00B710D0  cmp         dword ptr [ebp-0Ch],3E8h  
00B710D7  je          main+98h (0B71108h)       ; (input - 4) == 1000 이면 case 1000(0B71108h)으로 분기
00B710D9  cmp         dword ptr [ebp-0Ch],0C8h  
00B710E0  je          main+0B0h (0B71120h)      ; (input - 4) == 200 이면 case 200(0B71120h)으로 분기
00B710E2  cmp         dword ptr [ebp-0Ch],320h  
00B710E9  je          main+0A8h (0B71118h)      ; (input - 4) == 800 이면 case 800(0B71118h)으로 분기
00B710EB  jmp         $LN13 (0B71140h)          ; default 조건(0B71140h)으로 분기
00B710ED  cmp         dword ptr [ebp-0Ch],7D0h  
00B710F4  je          main+88h (0B710F8h)       ; (input - 4) == 2000 이면 case 2000(0B710F8h)으로 분기
00B710F6  jmp         $LN13 (0B71140h)          ; default 조건(0B71140h)으로 분기
{
case 2000:      // 00B710F8h
    output = input;
    break;
case 50:        // 00B71100h
    output = input;
    break;
case 1000:      // 00B71108h
    output = input;
    break; 
case 100:       // 00B71110h
    output = input;
    break; 
case 800:       // 00B71118h
    output = input;
    break;
case 200:       // 00B71120h
    output = input; 
    break;
case 4:         // 00B71128h
    output = input;
    break;
case 5:         // 00B71130h
    output = input;
    break;
case 7:         // 00B71138h
    output = input;
    break;
default:        // 00B71140h
    output = 0xFF; 
    break;
}
</code></pre>
## if 문
### 
