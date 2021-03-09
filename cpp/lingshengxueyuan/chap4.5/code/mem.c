

#include <string.h>
#include <openssl/crypto.h>

int main() {

	char *p = OPENSSL_malloc(4);

	p = OPENSSL_remalloc(p, 40);
	p = OPENSSL_realloc(p, 32);

	int i = 0;
	for (i = 0;i < 32;i ++) {
		memset(&p[i], i, 1);
	}

	p = OPENSSL_realloc_clean(p, 32, 77);
	p = OPENSSL_remalloc(p, 40);

	OPENSSL_malloc_locked(3);

	OPENSSL_free(p);

	return 0;
	
}



