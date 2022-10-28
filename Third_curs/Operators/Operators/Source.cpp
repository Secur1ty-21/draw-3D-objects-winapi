#include <stdio.h>
#include <cstring>
#include <stdlib.h>
#include <cmath>

struct MyStruct
{
	char *s;
};

struct MyStruct2
{
	int number;
};

MyStruct	operator&(MyStruct s1, MyStruct s2);
MyStruct2	operator*(MyStruct2 n1);

int	main(void){

	MyStruct s1;
	MyStruct s2;
	s1.s = (char*)"abcabcabcabcabc";
	s2.s = (char*)"bc";
	MyStruct s3 = s1 & s2;
	printf("%s\n", s3.s);
	free(s3.s);
	MyStruct2 n1;
	n1.number = 123456789;
	MyStruct2 n2 = *n1;
	printf("%d\n", n2.number);
	return (0);
}

MyStruct	operator &(MyStruct s1, MyStruct s2)
{
	int len = strlen(s1.s);
	int i = 0;
	int j = 0;
	while (s1.s[i])
	{
		while (s2.s[j])
		{
			if (s1.s[i] == s2.s[j])
			{
				len--;
				break;
			}
			j++;
		}
		j = 0;
		i++;
	}
	MyStruct s3;
	bool flag = true;
	s3.s = (char*)malloc(sizeof(char*) * (len + 1));
	i = 0;
	j = 0;
	len = 0;
	while (s1.s[i])
	{
		while (s2.s[j])
		{
			if (s1.s[i] == s2.s[j])
			{
				flag = false;
				break;
			}
			j++;
		}
		if (flag)
		{
			s3.s[len] = s1.s[i];
			len++;
		}
		else
			flag = true;
		j = 0;
		i++;
	}
	s3.s[len] = '\0';
	return (s3);
}

MyStruct2	operator*(MyStruct2 n1) {
	int step = 0;
	int nextDigit = 0;
	int numOfEvenDigit = 0;
	while (step < n1.number)
	{
		step = pow(10, nextDigit);
		if (n1.number / step % 10 % 2 == 0)
			numOfEvenDigit++;
		nextDigit++;
	}
	numOfEvenDigit--;
	MyStruct2 n2;
	n2.number = numOfEvenDigit;
	return (n2);
}