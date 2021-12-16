# 메모리 동적할당
## malloc
### Release 모드에서의 malloc
<pre><code>
///////////////////////////////////////// main 함수 내부
char* ch = (char*)malloc(sizeof(char) * BUF_SIZE);
00A31006  push        14h  										; 요청 할당 크기 
00A31008  call        dword ptr [__imp__malloc (0A32054h)]  	; malloc 함수 호출
00A3100E  add         esp,4  
00A31011  mov         dword ptr [ch],eax  

///////////////////////////////////////// dword ptr [__imp__malloc (0A32054h)]
76F30130  mov         edi,edi  
76F30132  push        ebp  
76F30133  mov         ebp,esp  
76F30135  pop         ebp  
76F30136  jmp         _malloc_base (76F30140h)  	; _malloc_base로 분기

///////////////////////////////////////// _malloc_base (76F30140h)
76F30140  mov         edi,edi  
76F30142  push        ebp  
76F30143  mov         ebp,esp  
76F30145  push        esi  
76F30146  mov         esi,dword ptr [ebp+8]  							; main에서 Push한 14h 값을 esi에 저장
76F30149  push        edi  
76F3014A  cmp         esi,0FFFFFFE0h  
76F3014D  ja          __XMMI_FP_Emulation+0A9AFh (76F777B2h)  
76F30153  test        esi,esi  
76F30155  je          _malloc_base+32h (76F30172h)  
76F30157  push        esi  
76F30158  push        0  
76F3015A  push        dword ptr [___acrt_heap (770112DCh)]  			; 힙 핸들
76F30160  call        dword ptr [__imp__HeapAlloc@12 (770120F4h)]  		; HeapAlloc 함수 호출. 왼쪽 순서대로 힙 핸들, 0, 20(요청 할당 크기)
76F30166  test        eax,eax  
76F30168  je          __XMMI_FP_Emulation+0AA5Eh (76F77861h)  
76F3016E  pop         edi  
76F3016F  pop         esi  
76F30170  pop         ebp  
76F30171  ret  
</code></pre>
* 결과: 함수 호출 2계층을 거쳐 힙 핸들을 이용하여 HeapAlloc을 호출한다.
* 작성하지는 않았지만 Debug 빌드 모드에서도 요청 할당 크기만큼 내부적으로 HeapAlloc을 호출한다.
### 실제 메모리 적재
<pre><code>
///////////////////////////////////////// Debug 빌드 모드
0x01584AE0  01 00 00 00  ....		; ..?
0x01584AE4  14 00 00 00  ....		; 동적 할당 전체 할당 크기 (프로그래머 영역만)
0x01584AE8  4b 00 00 00  K...		; Heap 관리자 데이터라고 분석됨
0x01584AEC  fd fd fd fd  ????		; Heap 관리자가 프로그래머 할당 메모리 앞뒤로 표시해둔 데이터라고 분석됨
0x01584AF0  aa aa aa aa  ????		; 할당 영역 시작
0x01584AF4  aa aa aa aa  ????
0x01584AF8  aa aa aa aa  ????
0x01584AFC  aa aa aa aa  ????
0x01584B00  aa aa aa aa  ????		; 할당 영역 끝
0x01584B04  fd fd fd fd  ????		
///////////////////////////////////////// Release 빌드 모드
0x00F60590  4b e5 75 af  K?u?
0x00F60594  fa 3f 00 0c  ??..
0x00F60598  aa aa aa aa  ????		; 할당 영역 시작
0x00F6059C  aa aa aa aa  ????
0x00F605A0  aa aa aa aa  ????
0x00F605A4  aa aa aa aa  ????
0x00F605A8  aa aa aa aa  ????		; 할당 영역 끝
0x00F605AC  00 00 00 00  ....
0x00F605B0  54 e5 75 b0  T?u?
0x00F605B4  e3 3f 00 08  ??..
</code></pre>
* 결과: Debug 모드의 경우 할당 주소 앞쪽에 동적 할당 전체 크기가 명시되어 있다. Release 모드에서는 명시되어 있지 않다. -> 그렇다면 Release 모드에서는 어떻게 크기를 알고 해제하는 것일까?
