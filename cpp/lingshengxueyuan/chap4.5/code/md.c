

#include <stdio.h>
#include <string.h>

#include <openssl/objects.h>
#include <openssl/rsa.h>


int main() {

	unsigned char in[] = "EVP_ENCODE_CTX ectx, dctx; \
		unsigned char in[100], base64[150], decode[100]; \
		EVP_EncodeInit(&ectx); \
		int i = 0; \
		for (i = 0;i < 100;i ++) { \
			in[i] = i;\
		} ";

		unsigned char out[128] = {0};

		int n = strlen(in);
		int i = 0;

		MD4(in, n, out);
		printf("MD4 result: \n"); // 16 byte 
		for (i = 0;i < 16;i ++) {
			printf("%x", out[i]);
		}
		printf("\n");

		MD5(in, n, out);
		printf("MD4 result: \n"); // 16 byte 
		for (i = 0;i < 16;i ++) {
			printf("%x", out[i]);
		}
		printf("\n");

		SHA(in, n, out);
		printf("MD4 result: \n"); // 20 byte 
		for (i = 0;i < 16;i ++) {
			printf("%x", out[i]);
		}
		printf("\n");


		SHA1(in, n, out);
		printf("MD4 result: \n"); // 20 byte 
		for (i = 0;i < 20;i ++) {
			printf("%x", out[i]);
		}
		printf("\n");

		
		SHA256(in, n, out);
		printf("MD4 result: \n"); // 32 byte 
		for (i = 0;i < 32;i ++) {
			printf("%x", out[i]);
		}
		printf("\n");

		
		SHA512(in, n, out);
		printf("MD4 result: \n"); // 64 byte 
		for (i = 0;i < 64;i ++) {
			printf("%x", out[i]);
		}
		printf("\n");

}




