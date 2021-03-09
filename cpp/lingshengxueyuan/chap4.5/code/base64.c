

#include <string.h>
#include <openssl/evp.h>


int main() {

	unsigned char in[100], base64[150], decode[100];

	EVP_ENCODE_CTX ectx;

	EVP_EncodeInit(&ectx);
	int i = 0;
	for (i = 0;i < 100;i ++) {
		in[i] = i;
	}

	int outl, inl = 100;
	EVP_EncodeUpdate(&ectx, base64, &outl, in, inl);

	int total = outl;
	EVP_EncodeFinal(&ectx, base64+total, &outl);

	printf("%s\n", base64);

}



