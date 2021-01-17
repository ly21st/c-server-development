#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "md5.h"  

int main(int argc, char *argv[])  
{  
	if (argc != 2)
	{
		printf("Usage: %s <str>\n", argv[0]);
		return -1;
	}
    MD5_CTX md5;  
    MD5Init(&md5);           
    int i;  
    //unsigned char encrypt[] ="admin";  
	unsigned char *encrypt = argv[1]; 
    unsigned char decrypt[16];      
    MD5Update(&md5,encrypt,strlen((char *)encrypt));  
    MD5Final(&md5,decrypt);   
	
    printf("加密前:%s\n加密后16位:",encrypt);  
    for(i=4;i<12;i++)  
    {  
        printf("%02x",decrypt[i]);  //02x前需要加上 %  
    }  
       
    printf("\n加密前:%s\n加密后32位:",encrypt);  
    for(i=0;i<16;i++)  
    {  
        printf("%02x",decrypt[i]);  //02x前需要加上 %  
    }  
  
    getchar();  
  
    return 0;  
}  