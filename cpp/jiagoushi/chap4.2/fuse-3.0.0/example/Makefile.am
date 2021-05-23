## Process this file with automake to produce Makefile.in

AM_CPPFLAGS = -I$(top_srcdir)/include -D_REENTRANT
noinst_HEADERS = ioctl.h
noinst_PROGRAMS = passthrough passthrough_fh hello hello_ll \
		  ioctl ioctl_client poll poll_client \
		  passthrough_ll notify_inval_inode \
		  notify_store_retrieve notify_inval_entry \
		  cuse cuse_client

LDADD = ../lib/libfuse3.la
passthrough_fh_LDADD = ../lib/libfuse3.la @passthrough_fh_libs@

ioctl_client_CPPFLAGS =
ioctl_client_LDFLAGS =
ioctl_client_LDADD =
poll_client_CPPFLAGS =
poll_client_LDFLAGS =
poll_client_LDADD =
e <sys/stat.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include "ioctl.h"

const char *usage =
"Usage: fioclient FIOC_FILE [size]\n"
"\n"
"Get size if <size> is omitted, set size otherwise\n"
"\n";

int main(int argc, char **argv)
{
	size_t size;
	int fd;

	if (argc < 2) {
		fprintf(stderr, "%s", usage);
		return 1;
	}

	fd = open(argv[1], O_RDWR);
	if (fd < 0) {
		perror("open");
		return 1;
	}

	if (argc == 2) {
		if (ioctl(fd, FIOC_GET_SIZE, &size)) {
			perror("ioctl");
			return 1;
		}
		printf("%zu\n", size);
	} else {
		size = strtoul(argv[2], NULL, 0);
		if (ioctl(fd, FIOC_SET_SIZE, &size)) {
			perror("ioctl");
			return 1;
		}
	}
	return 0;
}
