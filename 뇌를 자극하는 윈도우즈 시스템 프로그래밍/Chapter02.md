# Chapter02 아스키코드vs유니코드
## Section01 컴퓨터 구조와 프로그래밍 모델
1. 아스키 코드와 유니코드
	1) 아스키코드: 7bit로 표현되는 문자 매핑에 관한 코드
	2) 유니코드: 2byte로 표현되는 문자 매핑에 관한 코드
2. SBCS, MBCS, WBCS
	1) SBCS, MBCS, WBCS는 모두 문자를 표현하는데 정해진 바이트 수를 사용하는 "문자셋"에 대한 정의이다.
	2) SBCS: 모든 문자는 1byte 크기이다.
	3) MBCS: 각 언어별 문자의 크기가 1~2 byte의 크기를 가진다.(가변적)
	4) WBCS: 모든 문자가 2byte 크기이다. NULL 문자도 2byte이다.
3. MBCS 문자열의 문제점
	1) 각기 다른 언어에 대한 문자열 길이 계산이 다르다! 아스키코는 1의 길이로 그 외 문자는 2로 계산된다.
4. Windows 2000 이상의 OS에서는 내부적으로 모든 문자열을 유니코드 기반으로 처리한다.
	1) 따라서, 유니코드로 작성하면 윈도우에서 제공하는 라이브러리 함수 호출 시 문자 변환 과정이 없어진다.
5. wprintf, fputws와 같은 함수를 통해 '유니코드' 기반으로 '한글'을 출력하고자 한다면, _wsetlocale(LC_ALL, L"Korean")이 코드에 포함되어야 한다.
6. 프로그램 실행 시 "전달되는 명령행"을 유니코드로 전달
	1) 진입함수가 wmain이어야 프로그램 실행 명령행의 문자열(argv)이 유니코드로 전달된다.
	2) 진입함수 적용: main->MBCS, wmain->WBCS
7. 고찰
	1) 유니코드와 WBCS 그리고 인코딩 방식
		* 유니코드: 전 세계 모든 문자를 특정 숫자(키)와 1:1로 매핑한 코드이다. (WBCS와 유니코드를 혼동하면 안됨)
			1. UTF-8/UTF-16에서는 임의 문자에 대하여 유니코드 키를 표현할 때 U+XXXX(X는 16진수값)으로 표현한다. ex: 한글 '안'을 표현 시 U+C548
		* 인코딩: 파일을 저장할 때 파일 내 문자를 저장하는 방식으로 UTF-8, UTF-16(0xFEFF 파일 마커 존재), EUC-KR, CP949 등의 방식이 있다.
			1. 유니코드로 작성된 소스코드에서 파일 입출력 시 인코딩 방식을 고려하여 코드를 작성할 필요가 있다.
		* 참조: https://norux.me/31, https://ko.wikipedia.org/wiki/%EC%9C%A0%EB%8B%88%EC%BD%94%EB%93%9C_C000~CFFF
	2) WBCS 기반 문자열 조작 함수
		* 참조: https://makersweb.net/cpp/1394
		* 안전 문자열 함수에서 문자 사이즈에 대한 내용(TODO: 유니코드 기반으로 작성된 안전 문자열 함수 사용 시 버퍼 크기 입력에 관한 내용 추가)
## Section02 MBCS와 WBCS의 동시 지원
1. #include <windows.h>
	1) windows.h: Windows 기반 프로그래밍 시 기본적으로 항상 포함되어야 하는 헤더 파일이다.
2. Windows에서 정의하고 있는 자료형
	1) Windows에서는 typedef 키워드를 사용하여 자료형을 Windows 스타일로 정의한다.
		* 참조: https://docs.microsoft.com/ko-kr/windows/win32/winprog/windows-data-types
	2) 장점: 편의성(원본 자료형이 길 경우 간결하게 표현이 가능), 확장의 용이성(사용중인 자료형들을 일괄적으로 변경하여야하는 경우에 용이함)
		* 책에서 설명하는 편의성에 대한 장점은 Trade off가 있다고 생각한다. 정확히 어떤 자료형인지 추적하여야하기 때문이다.
	3) MBCS와 WBCS(유니코드)를 동시에 지원하기 위한 매크로
		* Windows에서는 MBCS와 WBCS를 동시에 수용하는 프로그램 구현을 위한 매크로를 지원한다. 이는 UNICODE 매크로의 정의를 기준으로 작성되고 tchar.h에 포함되어 있다. 본 책에서는 다음과 같은 예시를 소개한다.
		<pre>
		<code>
		#ifdef UNICODE 혹은 _UNICODE
			typedef WCHAR			TCHAR;
			typedef LPWSTR		LPTSTR;
			typedef LPCWSTR		LPCTSTR;
			#define __T(x)		L ## x
		#else
			typedef CHAR			TCHAR;
			typedef LPSTR		LPTSTR;
			typedef LPCSTR		LPCTSTR;
			#define __T(x)		x
		#endif
			#define _T(x)				__T(x)
		</pre>
		</code>
	2) Visual Studio에서는 기본적으로 전처리기 옵션에서 UNICODE, _UNICODE를 정의하고 있다.
4. MBCS와 WBCS를 동시에 지원하기 위한 함수들
	1) tchar.h에는 MBCS와 WBCS를 동시에 수용하는 프로그램을 위한 함수들을 제공한다.
	* 참조: https://blog.naver.com/PostView.naver?blogId=feelwoo&logNo=100036707000&parentCategoryNo=&categoryNo=12&viewDate=&isShowPopularPosts=true&from=search
