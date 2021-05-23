


#define FUSE_USE_VERSION 30

#include <fuse.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <time.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/uio.h>
#include <sys/ioctl.h>


enum {

	SAMPLE_NONE = 0,
	SAMPLE_ROOT,
	SAMPLE_FILE,

};


#define FILENAME_SAMPLE		"sample"
//#define FILENAME_NULL		"null"
//#define FILENAME_AWAKEN		"awaken"


static size_t sample_size = 0;
static void *sample_buffer;

static int sample_file_type(const char *path) {

	if (strcmp(path, "/") == 0) {
		return SAMPLE_ROOT;
	}
	if (strcmp(path, "/"FILENAME_SAMPLE) == 0) {
		return SAMPLE_FILE;
	}
	
	
	return SAMPLE_NONE;
}


// libfuse --> 

int sample_open (const char *path, struct fuse_file_info *fi) {

	return 0;

}


int sample_release (const char *path, struct fuse_file_info *fi) {

	return 0;
}

int sample_getattr (const char *path, struct stat *stbuf, struct fuse_file_info *fi) {

	stbuf->st_uid = getuid();
	stbuf->st_gid = getgid();

	stbuf->st_atime = stbuf->st_mtime = time(NULL);

	switch (sample_file_type(path)) {

		case SAMPLE_ROOT: {
			stbuf->st_mode = S_IFDIR | 0755;
			stbuf->st_nlink = 2;
			break;
		}
		
		case SAMPLE_FILE: {

			stbuf->st_mode = S_IFREG | 0777;
			stbuf->st_nlink = 1;
			stbuf->st_size = sample_size;

			break;
		}

		case SAMPLE_NONE: {
			return -ENOENT;
		}
	}
	return 0;
}


int sample_readdir (const char *path, void *buf, fuse_fill_dir_t filler, off_t offset,
			struct fuse_file_info *fi, enum fuse_readdir_flags flags) {

	filler(buf, ".", NULL, 0, 0);
	filler(buf, "..", NULL, 0, 0);
	filler(buf, FILENAME_SAMPLE, NULL, 0, 0);
	//filler(buf, FILENAME_NULL, NULL, 0, 0);
	//filler(buf, FILENAME_AWAKEN, NULL, 0, 0);

	return 0; //
}


static int sample_do_read(char *buf, size_t size, off_t offset) {

	if (size+offset > sample_size) {
		memcpy(buf, sample_buffer+offset, sample_size-offset);
		return sample_size-offset;
	}

	memcpy(buf, sample_buffer+offset, size);
	
	return size;
}


int sample_read (const char *path, char *buf, size_t size, off_t offset,
		     struct fuse_file_info *fi) {

	if (sample_file_type(path) != SAMPLE_FILE) {
		return -EINVAL;
	}

	return sample_do_read(buf, size, offset);
}


static int sample_resize(size_t new_size) {

	void *new_buf = realloc(sample_buffer, new_size);
	if (!new_buf) return -ENOMEM;

	if (new_size > sample_size) {
		memset(new_buf+sample_size, 0, new_size-sample_size);
	}

	sample_buffer = new_buf;
	sample_size = new_size;
	
	return 0;
}

static int sample_do_write(const char *buf, size_t size, off_t offset) {

	if (size+offset > sample_size) {
		if (0 != sample_resize(size+offset)) return -ENOMEM;
	}

	memcpy(sample_buffer+offset, buf, size);

	return size;
	
}

int sample_write (const char *path, const char *buf, size_t size, off_t offset,
		      struct fuse_file_info *fi) {

	// echo "nihao" > /mnt/sample
	// buf, size, offset

	if (sample_file_type(path) != SAMPLE_FILE) {
		return -EINVAL;
	}

	return sample_do_write(buf, size, offset);

}



static const struct fuse_operations sample_opera = {

	.open = sample_open,
#if 1
	.read = sample_read,
	.write = sample_write,
#endif
	.release = sample_release,
	.getattr = sample_getattr,
	.readdir = sample_readdir,
	
};

int main(int argc, char *argv[]) {

	return fuse_main(argc, argv, &sample_opera, NULL);

}


