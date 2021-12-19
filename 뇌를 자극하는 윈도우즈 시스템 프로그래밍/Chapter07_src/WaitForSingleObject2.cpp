#include <stdio.h>
#include <string.h>
#include <windows.h>

INT wmain(INT argc, WCHAR* argv[])
{
	INT ret = 0;
  
	if (argc == 3)
	{
		// 인자 개수 검사
		INT* num = (int*)malloc(sizeof(int) * (argc - 1));
		if (num == NULL)
			return 0;

		// ASCII to INTEGER
		for (int cnt = 1; cnt < argc; cnt++)
			num[cnt - 1] = _wtoi(argv[cnt]);

		// 총합 계산
		INT count = ((num[1] - num[0]) / 2);	// 총 개수 / 2
		INT spare = ((num[1] + num[0]) % 2);	// 개수의 짝수, 홀수 여부

		if (spare == 0)	// 개수가 짝수인 경우
			ret = (num[0] + num[1]) * count;
		else // 개수가 홀수인 경우
			ret = (num[0] + num[1]) * count + ((num[0] + num[1]) / 2);
	}

	return ret;
}
