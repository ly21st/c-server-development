#include <stdio.h>
#include <fcgi_stdio.h>

int main() {
    while (FCGI_Accept() >= 0) {
        printf(" Content-type: text/html\r\n");
        printf("\r\n");
        printf("hello cgi\n");
    }
    return 0;
}