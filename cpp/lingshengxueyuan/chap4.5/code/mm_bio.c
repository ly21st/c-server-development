

#include <stdio.h>
#include <openssl/bio.h>


int main() {

	BIO *b = BIO_new(BIO_s_mem()); // 

	int len = BIO_write(b, "openssl", 7);
	len = BIO_printf(b, "%s", "King");
	
	len = BIO_ctrl_pending(b); // mem length

	char out[128] = {0};
	len = BIO_read(b, out, len);
	printf("out: %s\n", out);

	BIO_free(b);

	return 0;
}


