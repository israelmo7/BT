#include "molib.h"

int mStrlen(char* s) //Unsecure
{
	int i=0;
	while(s[i]){i++;};
	
	return i;
	
}
void mStrcpy(char* dest, char* src, int len)
{
	int i =0;
	//printf("Strcpy: %s to ",dest);

	for(i =0; i < len; i++)
	{
		dest[i]=src[i];
		
	}
	dest[i] = 0;
	//printf(" %s\n",dest);
	
}
void mStrcat(char* dest, char* src, int len)
{
	int i =0,j = mStrlen(dest);
	

	//printf("len:: %d\n",j);
	
	for(i =0; i < len; i++)
	{
		dest[j++]=src[i];
		
	}
	dest[j] = 0;
	
	//printf("S %s\n",dest);
	
	
	
}
int lenOfInt(int num)
{
	int len = 0;
	
	while(num > 0)
	{
		num /= 10;
		len++;
	}
	return len;
}
char* mItoa(int num)
{
	int i=0,len = lenOfInt(num) + 1;
	char* result = (char*)malloc(sizeof(char)*len);
	
	while(num > 0)
	{
		result[len -1 - (++i)] = ('0'+num%10);
		
		num /= 10;
	}
	result[len-1] = 0;
	return result;
}
