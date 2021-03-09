

#include <stdio.h>
#include <openssl/bio.h>


int main() {

	BIO *b = BIO_new(BIO_s_mem());
	int len = BIO_write(b, "OpenSSL", 4);
	len = BIO_printf(b, "%s", "zcp");
	printf("len: %d\n", len);
	
	len = BIO_ctrl_pending(b);
	printf("len: %d\n", len);

	char *out = OPENSSL_malloc(len);
	len = BIO_read(b, out, len);
	printf("len: %d\n", len);

	OPENSSL_free(out);
	BIO_free(b);

	return 0;

}



