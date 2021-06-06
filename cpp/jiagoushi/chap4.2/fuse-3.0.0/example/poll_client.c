/*
  FUSE fselclient: FUSE select example client
  Copyright (C) 2008       SUSE Linux Products GmbH
  Copyright (C) 2008       Tejun Heo <teheo@suse.de>

  This program can be distributed under the terms of the GNU GPL.
  See the file COPYING.
*/

/** @file
 *
 * This program tests the poll.c example file systsem.
 *
 * Compile with:
 *
 *      gcc -Wall poll_client.c -o poll_client
 *
 * ## Source code ##
 * \include poll_client.c
 */

#include <config.h>

#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#define FSEL_FILES	16

int main(void)
{
	static const char hex_map[FSEL_FILES] = "0123456789ABCDEF";
	int fds[FSEL_FILES];
	int i, nfds, tries;

	for (i = 0; i < FSEL_FILES; i++) {
		char name[] = { hex_map[i], '\0' };
		fds[i] = open(name, O_RDONLY);
		if (fds[i] < 0) {
			perror("open");
			return 1;
		}
	}
	nfds = fds[FSEL_FILES - 1] + 1;

	for(tries=0; tries < 16; tries++) {
		static char buf[4096];
		fd_set rfds;
		int rc;

		FD_ZERO(&rfds);
		for (i = 0; i < FSEL_FILES; i++)
			FD_SET(fds[i], &rfds);

		rc = select(nfds, &rfds, NULL, NULL, NULL);

		if (rc < 0) {
			perror("select");
			return 1;
		}

		for (i = 0; i < FSEL_FILES; i++) {
			if (!FD_ISSET(fds[i], &rfds)) {
				printf("_:   ");
				continue;
			}
			printf("%X:", i);
			rc = read(fds[i], buf, sizeof(buf));
			if (rc < 0) {
				perror("read");
				return 1;
			}
			printf("%02d ", rc);
		}
		printf("\n");
	}
}
 nbytes stored in each file */

static int fsel_path_index(const char *path)
{
	char ch = path[1];

	if (strlen(path) != 2 || path[0] != '/' || !isxdigit(ch) || islower(ch))
		return -1;
	return ch <= '9' ? ch - '0' : ch - 'A' + 10;
}

static int fsel_getattr(const char *path, struct stat *stbuf,
			struct fuse_file_info *fi)
{
	(void) fi;
	int idx;

	memset(stbuf, 0, sizeof(struct stat));

	if (strcmp(path, "/") == 0) {
		stbuf->st_mode = S_IFDIR | 0555;
		stbuf->st_nlink = 2;
		return 0;
	}

	idx = fsel_path_index(path);
	if (idx < 0)
		return -ENOENT;

	stbuf->st_mode = S_IFREG | 0444;
	stbuf->st_nlink = 1;
	stbuf->st_size = fsel_cnt[idx];
	return 0;
}

static int fsel_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
			off_t offset, struct fuse_file_info *fi,
			enum fuse_readdir_flags flags)
{
	char name[2] = { };
	int i;

	(void) offset;
	(void) fi;
	(void) flags;

	if (strcmp(path, "/") != 0)
		return -ENOENT;

	for (i = 0; i < FSEL_FILES; i++) {
		name[0] = fsel_hex_map[i];
		filler(buf, name, NULL, 0, 0);
	}

	return 0;
}

static int fsel_open(const char *path, struct fuse_file_info *fi)
{
	int idx = fsel_path_index(path);

	if (idx < 0)
		return -ENOENT;
	if ((fi->flags & 3) != O_RDONLY)
		return -EACCES;
	if (fsel_open_mask & (1 << idx))
		return -EBUSY;
	fsel_open_mask |= (1 << idx);

	/*
	 * fsel files are nonseekable somewhat pipe-like files which
	 * gets filled up periodically by producer thread and consumed
	 * on read.  Tell FUSE as such.
	 */
	fi->fh = idx;
	fi->direct_io = 1;
	fi->nonseekable = 1;

	return 0;
}

static int fsel_release(const char *path, struct fuse_file_info *fi)
{
	int idx = fi->fh;

	(void) path;

	fsel_open_mask &= ~(1 << idx);
	return 0;
}

static int fsel_read(const char *path, char *buf, size_t size, off_t offset,
		     struct fuse_file_info *fi)
{
	int idx = fi->fh;

	(void) path;
	(void) offset;

	pthread_mutex_lock(&fsel_mutex);
	if (fsel_cnt[idx] < size)
		size = fsel_cnt[idx];
	printf("READ   %X transferred=%zu cnt=%u\n", idx, size, fsel_cnt[idx]);
	fsel_cnt[idx] -= size;
	pthread_mutex_unlock(&fsel_mutex);

	memset(buf, fsel_hex_map[idx], size);
	return size;
}

static int fsel_poll(const char *path, struct fuse_file_info *fi,
		     struct fuse_pollhandle *ph, unsigned *reventsp)
{
	static unsigned polled_zero;
	int idx = fi->fh;

	(void) path;

	/*
	 * Poll notification requires pointer to struct fuse which
	 * can't be obtained when using fuse_main().  As notification
	 * happens only after poll is called, fill it here from
	 * fuse_context.
	 */
	if (!fsel_fuse) {
		struct fuse_context *cxt = fuse_get_context();
		if (cxt)
			fsel_fuse = cxt->fuse;
	}

	pthread_mutex_lock(&fsel_mutex);

	if (ph != NULL) {
		struct fuse_pollhandle *oldph = fsel_poll_handle[idx];

		if (oldph)
			fuse_pollhandle_destroy(oldph);

		fsel_poll_notify_mask |= (1 << idx);
		fsel_poll_handle[idx] = ph;
	}

	if (fsel_cnt[idx]) {
		*reventsp |= POLLIN;
		printf("POLL   %X cnt=%u polled_zero=%u\n",
		       idx, fsel_cnt[idx], polled_zero);
		polled_zero = 0;
	} else
		polled_zero++;

	pthread_mutex_unlock(&fsel_mutex);
	return 0;
}

static struct fuse_operations fsel_oper = {
	.getattr	= fsel_getattr,
	.readdir	= fsel_readdir,
	.open		= fsel_open,
	.release	= fsel_release,
	.read		= fsel_read,
	.poll		= fsel_poll,
};

static void *fsel_producer(void *data)
{
	const struct timespec interval = { 0, 250000000 };
	unsigned idx = 0, nr = 1;

	(void) data;

	while (1) {
		int i, t;

		pthread_mutex_lock(&fsel_mutex);

		/*
		 * This is the main producer loop which is executed
		 * ever 500ms.  On each iteration, it fills one byte
		 * to 1, 2 or 4 files and sends poll notification if
		 * requested.
		 */
		for (i = 0, t = idx; i < nr;
		     i++, t = (t + FSEL_FILES / nr) % FSEL_FILES) {
			if (fsel_cnt[t] == FSEL_CNT_MAX)
				continue;

			fsel_cnt[t]++;
			if (fsel_fuse && (fsel_poll_notify_mask & (1 << t))) {
				struct fuse_pollhandle *ph;

				printf("NOTIFY %X\n", t);
				ph = fsel_poll_handle[t];
				fuse_notify_poll(ph);
				fuse_pollhandle_destroy(ph);
				fsel_poll_notify_mask &= ~(1 << t);
				fsel_poll_handle[t] = NULL;
			}
		}

		idx = (idx + 1) % FSEL_FILES;
		if (idx == 0)
			nr = (nr * 2) % 7;	/* cycle through 1, 2 and 4 */

		pthread_mutex_unlock(&fsel_mutex);

		nanosleep(&interval, NULL);
	}

	return NULL;
}

int main(int argc, char *argv[])
{
	pthread_t producer;
	pthread_attr_t attr;
	int ret;

	errno = pthread_mutex_init(&fsel_mutex, NULL);
	if (errno) {
		perror("pthread_mutex_init");
		return 1;
	}

	errno = pthread_attr_init(&attr);
	if (errno) {
		perror("pthread_attr_init");
		return 1;
	}

	errno = pthread_create(&producer, &attr, fsel_producer, NULL);
	if (errno) {
		perror("pthread_create");
		return 1;
	}

	ret = fuse_main(argc, argv, &fsel_oper, NULL);

	pthread_cancel(producer);
	pthread_join(producer, NULL);

	return ret;
}