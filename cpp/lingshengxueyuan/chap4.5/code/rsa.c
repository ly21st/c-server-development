



#include <openssl/rsa.h>

int main() {

	RSA *r;

	int bits = 512, ret;
	unsigned long e = RSA_3;

	BIGNUM *bne;

	r = RSA_generate_key(bits, e, NULL, NULL);
	RSA_print_fp(stdout, r, 11);
	RSA_free(r);

	//printf("\n\n----------------------------------\n\n");
	bne = BN_new();
	ret = BN_set_word(bne, e);

	//printf("\n\n----------------------------------\n\n");
	r = RSA_new();
	RSA_generate_key_ex(r, bits, bne, NULL);
	if (ret != 1) {
		printf("RSA_generate_key_ex err!\n");
		return -1;
	}
	
	RSA_free(r);
	
	return 0;
}



