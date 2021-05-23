/*
  FUSE: Filesystem in Userspace
  Copyright (C) 2001-2007  Miklos Szeredi <miklos@szeredi.hu>

  This program can be distributed under the terms of the GNU LGPLv2.
  See the file COPYING.LIB.
*/

#ifndef FUSE_LOWLEVEL_H_
#define FUSE_LOWLEVEL_H_

/** @file
 *
 * Low level API
 *
 * IMPORTANT: you should define FUSE_USE_VERSION before including this
 * header.  To use the newest API define it to 30 (recommended for any
 * new application).
 */

#ifndef FUSE_USE_VERSION
#define FUSE_USE_VERSION 30
#endif

#include "fuse_common.h"

#include <utime.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/statvfs.h>
#include <sys/uio.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ----------------------------------------------------------- *
 * Miscellaneous definitions				       *
 * ----------------------------------------------------------- */

/** The node ID of the root inode */
#define FUSE_ROOT_ID 1

/** Inode number type */
typedef uint64_t fuse_ino_t;

/** Request pointer type */
typedef struct fuse_req *fuse_req_t;

/**
 * Session
 *
 * This provides hooks for processing requests, and exiting
 */
struct fuse_session;

/** Directory entry parameters supplied to fuse_reply_entry() */
struct fuse_entry_param {
	/** Unique inode number
	 *
	 * In lookup, zero means negative entry (from version 2.5)
	 * Returning ENOENT also means negative entry, but by setting zero
	 * ino the kernel may cache negative entries for entry_timeout
	 * seconds.
	 */
	fuse_ino_t ino;

	/** Generation number for this entry.
	 *
	 * If the file system will be exported over NFS, the
	 * ino/generation pairs need to be unique over the file
	 * system's lifetime (rather than just the mount time). So if
	 * the file system reuses an inode after it has been deleted,
	 * it must assign a new, previously unused generation number
	 * to the inode at the same time.
	 *
	 * The generation must be non-zero, otherwise FUSE will treat
	 * it as an error.
	 *
	 */
	uint64_t generation;

	/** Inode attributes.
	 *
	 * Even if attr_timeout == 0, attr must be correct. For example,
	 * for open(), FUSE uses attr.st_size from lookup() to determine
	 * how many bytes to request. If this value is not correct,
	 * incorrect data will be returned.
	 */
	struct stat attr;

	/** Validity timeout (in seconds) for the attributes */
	double attr_timeout;

	/** Validity timeout (in seconds) for the name */
	double entry_timeout;
};

/**
 * Additional context associated with requests.
 *
 * Note that the reported client uid, gid and pid may be zero in some
 * situations. For example, if the FUSE file system is running in a
 * PID or user namespace but then accessed from outside the namespace,
 * there is no valid uid/pid/gid that could be reported.
 */
struct fuse_ctx {
	/** User ID of the calling process */
	uid_t uid;

	/** Group ID of the calling process */
	gid_t gid;

	/** Thread ID of the calling process */
	pid_t pid;

	/** Umask of the calling process */
	mode_t umask;
};

struct fuse_forget_data {
	fuse_ino_t ino;
	uint64_t nlookup;
};

/* 'to_set' flags in setattr */
#define FUSE_SET_ATTR_MODE	(1 << 0)
#define FUSE_SET_ATTR_UID	(1 << 1)
#define FUSE_SET_ATTR_GID	(1 << 2)
#define FUSE_SET_ATTR_SIZE	(1 << 3)
#define FUSE_SET_ATTR_ATIME	(1 << 4)
#define FUSE_SET_ATTR_MTIME	(1 << 5)
#define FUSE_SET_ATTR_ATIME_NOW	(1 << 7)
#define FUSE_SET_ATTR_MTIME_NOW	(1 << 8)
#define FUSE_SET_ATTR_CTIME	(1 << 10)

/* ----------------------------------------------------------- *
 * Request methods and replies				       *
 * ----------------------------------------------------------- */

/**
 * Low level filesystem operations
 *
 * Most of the methods (with the exception of init and destroy)
 * receive a request handle (fuse_req_t) as their first argument.
 * This handle must be passed to one of the specified reply functions.
 *
 * This may be done inside the method invocation, or after the call
 * has returned.  The request handle is valid until one of the reply
 * functions is called.
 *
 * Other pointer arguments (name, fuse_file_info, etc) are not valid
 * after the call has returned, so if they are needed later, their
 * contents have to be copied.
 *
 * In general, all methods are expected to perform any necessary
 * permission checking. However, a filesystem may delegate this task
 * to the kernel by passing the `default_permissions` mount option to
 * `fuse_session_new()`. In this case, methods will only be called if
 * the kernel's permission check has succeeded.
 *
 * The filesystem sometimes needs to handle a return value of -ENOENT
 * from the reply function, which means, that the request was
 * interrupted, and the reply discarded.  For example if
 * fuse_reply_open() return -ENOENT means, that the release method for
 * this file will not be called.
 */
struct fuse_lowlevel_ops {
	/**
	 * Initialize filesystem
	 *
	 * This function is called when libfuse establishes
	 * communication with the FUSE kernel module. The file system
	 * should use this module to inspect and/or modify the
	 * connection parameters provided in the `conn` structure.
	 *
	 * Note that some parameters may be overwritten by options
	 * passed to fuse_session_new() which take precedence over the
	 * values set in this handler.
	 *
	 * There's no reply to this function
	 *
	 * @param userdata the user data passed to fuse_session_new()
	 */
	void (*init) (void *userdata, struct fuse_conn_info *conn);

	/**
	 * Clean up filesystem
	 *
	 * Called on filesystem exit
	 *
	 * There's no reply to this function
	 *
	 * @param userdata the user data passed to fuse_session_new()
	 */
	void (*destroy) (void *userdata);

	/**
	 * Look up a directory entry by name and get its attributes.
	 *
	 * Valid replies:
	 *   fuse_reply_entry
	 *   fuse_reply_err
	 *
	 * @param req request handle
	 * @param parent inode number of the parent directory
	 * @param name the name to look up
	 */
	void (*lookup) (fuse_req_t req, fuse_ino_t parent, const char *name);

	/**
	 * Forget about an inode
	 *
	 * This function is called when the kernel removes an inode
	 * from its internal caches.
	 *
	 * The inode's lookup count increases by one for every call to
	 * fuse_reply_entry and fuse_reply_create. The nlookup parameter
	 * indicates by how much the lookup count should be decreased.
	 *
	 * Inodes with a non-zero lookup count may receive request from
	 * the kernel even after calls to unlink, rmdir or (when
	 * overwriting an existing file) rename. Filesystems must handle
	 * such requests properly and it is recommended to defer removal
	 * of the inode until the lookup count reaches zero. Calls to
	 * unlink, remdir or rename will be followed closely by forget
	 * unless the file or directory is open, in which case the
	 * kernel issues forget only after the release or releasedir
	 * calls.
	 *
	 * Note that if a file system will be exported over NFS the
	 * inodes lifetime must extend even beyond forget. See the
	 * generation field in struct fuse_entry_param above.
	 *
	 * On unmount the lookup count for all inodes implicitly drops
	 * to zero. It is not guaranteed that the file system will
	 * receive corresponding forget messages for the affected
	 * inodes.
	 *
	 * Valid replies:
	 *   fuse_reply_none
	 *
	 * @param req request handle
	 * @param ino the inode number
	 * @param nlookup the number of lookups to forget
	 */
	void (*forget) (fuse_req_t req, fuse_ino_t ino, uint64_t nlookup);

	/**
	 * Get file attributes.
	 *
	 * If writeback caching is enabled, the kernel may have a
	 * better idea of a file's length than the FUSE file system
	 * (eg if there has been a write that extended the file size,
	 * but that has not yet been passed to the filesystem.n
	 *
	 * In this case, the st_size value provided by the file system
	 * will be ignored.
	 *
	 * Valid replies:
	 *   fuse_reply_attr
	 *   fuse_reply_err
	 *
	 * @param req request handle
	 * @param ino the inode number
	 * @param fi for future use, currently always NULL
	 */
	void (*getattr) (fuse_req_t req, fuse_ino_t ino,
			 struct fuse_file_info *fi);

	/**
	 * Set file attributes
	 *
	 * In the 'attr' argument only members indicated by the 'to_set'
	 * bitmask contain valid values.  Other members contain undefined
	 * values.
	 *
	 * Unless FUSE_CAP_HANDLE_KILLPRIV is disabled, this method is
	 * expected to reset the setuid and setgid bits if the file
	 * size or owner is being changed.
	 *
	 * If the setattr was invoked from the ftruncate() system call
	 * under Linux kernel versions 2.6.15 or later, the fi->fh will
	 * contain the value set by the open method or will be undefined
	 * if the open method didn't set any value.  Otherwise (not
	 * ftruncate call, or kernel version earlier than 2.6.15) the fi
	 * parameter will be NULL.
	 *
	 * Valid replies:
	 *   fuse_reply_attr
	 *   fuse_reply_err
	 *
	 * @param req request handle
	 * @param ino the inode number
	 * @param attr the attributes
	 * @param to_set bit mask of attributes which should be set
	 * @param fi file information, or NULL
	 */
	void (*setattr) (fuse_req_t req, fuse_ino_t ino, struct stat *attr,
			 int to_set, struct fuse_file_info *fi);

	/**
	 * Read symbolic link
	 *
	 * Valid replies:
	 *   fuse_reply_readlink
	 *   fuse_reply_err
	 *
	 * @param req request handle
	 * @param ino the inode number
	 */
	void (*readlink) (fuse_req_t req, fuse_ino_t ino);

	/**
	 * Create file node
	 *
	 * Create a regular file, character device, block device, fifo or
	 * socket node.
	 *
	 * Valid replies:
	 *   fuse_reply_entry
	 *   fuse_reply_err
	 *
	 * @param req request handle
	 * @param parent inode number of the parent directory
	 * @param name to create
	 * @param mode file type and mode with which to create the new file
	 * @param rdev the device number (only valid if created file is a device)
	 */
	void (*mknod) (fuse_req_t req, fuse_ino_t parent, const char *name,
		       mode_t mode, dev_t rdev);

	/**
	 * Create a directory
	 *
	 * Valid replies:
	 *   fuse_reply_entry
	 *   fuse_reply_err
	 *
	 * @param req request handle
	 * @param parent inode number of the parent directory
	 * @param name to create
	 * @param mode with which to create the new file
	 */
	void (*mkdir) (fuse_req_t req, fuse_ino_t parent, const char *name,
		       mode_t mode);

	/**
	 * Remove a file
	 *
	 * If the file's inode's lookup count is non-zero, the file
	 * system is expected to postpone any removal of the inode
	 * until the lookup count reaches zero (see description of the
	 * forget function).
	 *
	 * Valid replies:
	 *   fuse_reply_err
	 *
	 * @param req request handle
	 * @param parent inode number of the parent directory
	 * @param name to remove
	 */
	void (*unlink) (fuse_req_t req, fuse_ino_t parent, const char *name);

	/**
	 * Remove a directory
	 *
	 * If the directory's inode's lookup count is non-zero, the
	 * file system is expected to postpone any removal of the
	 * inode until the lookup count reaches zero (see description
	 * of the forget function).
	 *
	 * Valid replies:
	 *   fuse_reply_err
	 *
	 * @param req request handle
	 * @param parent inode number of the parent directory
	 * @param name to remove
	 */
	void (*rmdir) (fuse_req_t req, fuse_ino_t parent, const char *name);

	/**
	 * Create a symbolic link
	 *
	 * Valid replies:
	 *   fuse_reply_entry
	 *   fuse_reply_err
	 *
	 * @param req request handle
	 * @param link the contents of the symbolic link
	 * @param parent inode number of the parent directory
	 * @param name to create
	 */
	void (*symlink) (fuse_req_t req, const char *link, fuse_ino_t parent,
			 const char *name);

	/** Rename a file
	 *
	 * If the target exists it should be atomically replaced. If
	 * the target's inode's lookup count is non-zero, the file
	 * system is expected to postpone any removal of the inode
	 * until the lookup count reaches zero (see description of the
	 * forget function).
	 *
	 * If this request is answered with an error code of ENOSYS, this is
	 * treated as a permanent failure with error code EINVAL, i.e. all
	 * future bmap requests will fail with EINVAL without being
	 * send to the filesystem process.
	 *
	 * Valid replies:
	 *   fuse_reply_err
	 *
	 * @param req request handle
	 * @param parent inode number of the old parent directory
	 * @param name old name
	 * @param newparent inode number of the new parent directory
	 * @param newname new name
	 */
	void (*rename) (fuse_req_t req, fuse_ino_t parent, const char *name,
			fuse_ino_t newparent, const char *newname,
			unsigned int flags);

	/**
	 * Create a hard link
	 *
	 * Valid replies:
	 *   fuse_reply_entry
	 *   fuse_reply_err
	 *
	 * @param req request handle
	 * @param ino the old inode number
	 * @param newparent inode number of the new parent directory
	 * @param newname new name to create
	 */
	void (*link) (fuse_req_t req, fuse_ino_t ino, fuse_ino_t newparent,
		      const char *newname);

	/**
	 * Open a file
	 *
	 * Open flags are available in fi->flags.  Creation (O_CREAT,
	 * O_EXCL, O_NOCTTY) and by default also truncation (O_TRUNC)
	 * flags will be filtered out. If an application specifies
	 * O_TRUNC, fuse first calls truncate() and then open().
	 *
	 * If filesystem is able to handle O_TRUNC directly, the
	 * init() handler should set the `FUSE_CAP_ATOMIC_O_TRUNC` bit
	 * in ``conn->want``.
	 *
	 * Filesystem may store an arbitrary file handle (pointer,
	 * index, etc) in fi->fh, and use this in other all other file
	 * operations (read, write, flush, release, fsync).
	 *
	 * Filesystem may also implement stateless file I/O and not store
	 * anything in fi->fh.
	 *
	 * There are also some flags (direct_io, keep_cache) which the
	 * filesystem may set in fi, to change the way the file is opened.
	 * See fuse_file_info structure in <fuse_common.h> for more details.
	 *
	 * If this request is answered with an error code of ENOSYS
	 * and FUSE_CAP_NO_OPEN_SUPPORT is set in
	 * `fuse_conn_info.capable`, this is treated as success and
	 * future calls to open will also succeed without being send
	 * to the filesystem process.
	 *
	 * Valid replies:
	 *   fuse_reply_open
	 *   fuse_reply_err
	 *
	 * @param req request handle
	 * @param ino the inode number
	 * @param fi file information
	 */
	void (*open) (fuse_req_t req, fuse_ino_t ino,
		      struct fuse_file_info *fi);

	/**
	 * Read data
	 *
	 * Read should send exactly the number of bytes requested except
	 * on EOF or error, otherwise the rest of the data will be
	 * substituted with zeroes.  An exception to this is when the file
	 * has been opened in 'direct_io' mode, in which case the return
	 * value of the read system call will reflect the return value of
	 * this operation.
	 *
	 * fi->fh will contain the value set by the open method, or will
	 * be undefined if the open method didn't set any value.
	 *
	 * Valid replies:
	 *   fuse_reply_buf
	 *   fuse_reply_iov
	 *   fuse_reply_data
	 *   fuse_reply_err
	 *
	 * @param req request handle
	 * @param ino the inode number
	 * @param size number of bytes to read
	 * @param off offset to read from
	 * @param fi file information
	 */
	void (*read) (fuse_req_t req, fuse_ino_t ino, size_t size, off_t off,
		      struct fuse_file_info *fi);

	/**
	 * Write data
	 *
	 * Write should return exactly the number of bytes requested
	 * except on error.  An exception to this is when the file has
	 * been opened in 'direct_io' mode, in which case the return value
	 * of the write system call will reflect the return value of this
	 * operation.
	 *
	 * Unless FUSE_CAP_HANDLE_KILLPRIV is disabled, this method is
	 * expected to reset the setuid and setgid bits.
	 *
	 * fi->fh will contain the value set by the open method, or will
	 * be undefined if the open method didn't set any value.
	 *
	 * Valid replies:
	 *   fuse_reply_write
	 *   fuse_reply_err
	 *
	 * @param req request handle
	 * @param ino the inode number
	 * @param buf data to write
	 * @param size number of bytes to write
	 * @param off offset to write to
	 * @param fi file information
	 */
	void (*write) (fuse_req_t req, fuse_ino_t ino, const char *buf,
		       size_t size, off_t off, struct fuse_file_info *fi);

	/**
	 * Flush method
	 *
	 * This is called on each close() of the opened file.
	 *
	 * Since file descriptors can be duplicated (dup, dup2, fork), for
	 * one open call there may be many flush calls.
	 *
	 * Filesystems shouldn't assume that flush will always be called
	 * after some writes, or that if will be called at all.
	 *
	 * fi->fh will contain the value set by the open method, or will
	 * be undefined if the open method didn't set any value.
	 *
	 * NOTE: the name of the method is misleading, since (unlike
	 * fsync) the filesystem is not forced to flush pending writes.
	 * One reason to flush data, is if the filesystem wants to return
	 * write errors.
	 *
	 * If the filesystem supports file locking operations (setlk,
	 * getlk) it should remove all locks belonging to 'fi->owner'.
	 *
	 * If this request is answered with an error code of ENOSYS,
	 * this is treated as success and future calls to flush() will
	 * succeed automatically without being send to the filesystem
	 * process.
	 *
	 * Valid replies:
	 *   fuse_reply_err
	 *
	 * @param req request handle
	 * @param ino the inode number
	 * @param fi file information
	 */
	void (*flush) (fuse_req_t req, fuse_ino_t ino,
		       struct fuse_file_info *fi);

	/**
	 * Release an open file
	 *
	 * Release is called when there are no more references to an open
	 * file: all file descriptors are closed and all memory mappings
	 * are unmapped.
	 *
	 * For every open call there will be exactly one release call.
	 *
	 * The filesystem may reply with an error, but error values are
	 * not returned to close() or munmap() which triggered the
	 * release.
	 *
	 * fi->fh will contain the value set by the open method, or will
	 * be undefined if the open method didn't set any value.
	 * fi->flags will contain the same flags as for open.
	 *
	 * Valid replies:
	 *   fuse_reply_err
	 *
	 * @param req request handle
	 * @param ino the inode number
	 * @param fi file information
	 */
	void (*release) (fuse_req_t req, fuse_ino_t ino,
			 struct fuse_file_info *fi);

	/**
	 * Synchronize file contents
	 *
	 * If the datasync parameter is non-zero, then only the user data
	 * should be flushed, not the meta data.
	 *
	 * If this request is answered with an error code of ENOSYS,
	 * this is treated as success and future calls to fsync() will
	 * succeed automatically without being send to the filesystem
	 * process.
	 *
	 * Valid replies:
	 *   fuse_reply_err
	 *
	 * @param req request handle
	 * @param ino the inode number
	 * @param datasync flag indicating if only data should be flushed
	 * @param fi file information
	 */
	void (*fsync) (fuse_req_t req, fuse_ino_t ino, int datasync,
		       struct fuse_file_info *fi);

	/**
	 * Open a directory
	 *
	 * Filesystem may store an arbitrary file handle (pointer, index,
	 * etc) in fi->fh, and use this in other all other directory
	 * stream operations (readdir, releasedir, fsyncdir).
	 *
	 * Filesystem may also implement stateless directory I/O and not
	 * store anything in fi->fh, though that makes it impossible to
	 * implement standard conforming directory stream operations in
	 * case the contents of the directory can change between opendir
	 * and releasedir.
	 *
	 * Valid replies:
	 *   fuse_reply_open
	 *   fuse_reply_err
	 *
	 * @param req request handle
	 * @param ino the inode number
	 * @param fi file information
	 */
	void (*opendir) (fuse_req_t req, fuse_ino_t ino,
			 struct fuse_file_info *fi);

	/**
	 * Read directory
	 *
	 * Send a buffer filled using fuse_add_direntry(), with size not
	 * exceeding the requested size.  Send an empty buffer on end of
	 * stream.
	 *
	 * fi->fh will contain the value set by the opendir method, or
	 * will be undefined if the opendir method didn't set any value.
	 *
	 * Returning a directory entry from readdir() does not affect
	 * its lookup count.
	 *
	 * The function does not have to report the '.' and '..'
	 * entries, but is allowed to do so.
	 *
	 * Valid replies:
	 *   fuse_reply_buf
	 *   fuse_reply_data
	 *   fuse_reply_err
	 *
	 * @param req request handle
	 * @param ino the inode number
	 * @param size maximum number of bytes to send
	 * @param off offset to continue reading the directory stream
	 * @param fi file information
	 */
	void (*readdir) (fuse_req_t req, fuse_ino_t ino, size_t size, off_t off,
			 struct fuse_file_info *fi);

	/**
	 * Release an open directory
	 *
	 * For every opendir call there will be exactly one releasedir
	 * call.
	 *
	 * fi->fh will contain the value set by the opendir method, or
	 * will be undefined if the opendir method didn't set any value.
	 *
	 * Valid replies:
	 *   fuse_reply_err
	 *
	 * @param req request handle
	 * @param ino the inode number
	 * @param fi file information
	 */
	void (*releasedir) (fuse_req_t req, fuse_ino_t ino,
			    struct fuse_file_info *fi);

	/**
	 * Synchronize directory contents
	 *
	 * If the datasync parameter is non-zero, then only the directory
	 * contents should be flushed, not the meta data.
	 *
	 * fi->fh will contain the value set by the opendir method, or
	 * will be undefined if the opendir method didn't set any value.
	 *
	 * If this request is answered with an error code of ENOSYS,
	 * this is treated as success and future calls to fsyncdir() will
	 * succeed automatically without being send to the filesystem
	 * process.
	 *
	 * Valid replies:
	 *   fuse_reply_err
	 *
	 * @param req request handle
	 * @param ino the inode number
	 * @param datasync flag indicating if only data should be flushed
	 * @param fi file information
	 */
	void (*fsyncdir) (fuse_req_t req, fuse_ino_t ino, int datasync,
			  struct fuse_file_info *fi);

	/**
	 * Get file system statistics
	 *
	 * Valid replies:
	 *   fuse_reply_statfs
	 *   fuse_reply_err
	 *
	 * @param req request handle
	 * @param ino the inode number, zero means "undefined"
	 */
	void (*statfs) (fuse_req_t req, fuse_ino_t ino);

	/**
	 * Set an extended attribute
	 *
	 * If this request is answered with an error code of ENOSYS, this is
	 * treated as a permanent failure with error code EOPNOTSUPP, i.e. all
	 * future setxattr() requests will fail with EOPNOTSUPP without being
	 * send to the filesystem process.
	 *
	 * Valid replies:
	 *   fuse_reply_err
	 */
	void (*setxattr) (fuse_req_t req, fuse_ino_t ino, const char *name,
			  const char *value, size_t size, int flags);

	/**
	 * Get an extended attribute
	 *
	 * If size is zero, the size of the value should be sent with
	 * fuse_reply_xattr.
	 *
	 * If the size is non-zero, and the value fits in the buffer, the
	 * value should be sent with fuse_reply_buf.
	 *
	 * If the size is too small for the value, the ERANGE error should
	 * be sent.
	 *
	 * If this request is answered with an error code of ENOSYS, this is
	 * treated as a permanent failure with error code EOPNOTSUPP, i.e. all
	 * future getxattr() requests will fail with EOPNOTSUPP without being
	 * send to the filesystem process.
	 *
	 * Valid replies:
	 *   fuse_reply_buf
	 *   fuse_reply_data
	 *   fuse_reply_xattr
	 *   fuse_reply_err
	 *
	 * @param req request handle
	 * @param ino the inode number
	 * @param name of the extended attribute
	 * @param size maximum size of the value to send
	 */
	void (*getxattr) (fuse_req_t req, fuse_ino_t ino, const char *name,
			  size_t size);

	/**
	 * List extended attribute names
	 *
	 * If size is zero, the total size of the attribute list should be
	 * sent with fuse_reply_xattr.
	 *
	 * If the size is non-zero, and the null character separated
	 * attribute list fits in the buffer, the list should be sent with
	 * fuse_reply_buf.
	 *
	 * If the size is too small for the list, the ERANGE error should
	 * be sent.
	 *
	 * If this request is answered with an error code of ENOSYS, this is
	 * treated as a permanent failure with error code EOPNOTSUPP, i.e. all
	 * future listxattr() requests will fail with EOPNOTSUPP without being
	 * send to the filesystem process.
	 *
	 * Valid replies:
	 *   fuse_reply_buf
	 *   fuse_reply_data
	 *   fuse_reply_xattr
	 *   fuse_reply_err
	 *
	 * @param req request handle
	 * @param ino the inode number
	 * @param size maximum size of the list to send
	 */
	void (*listxattr) (fuse_req_t req, fuse_ino_t ino, size_t size);

	/**
	 * Remove an extended attribute
	 *
	 * If this request is answered with an error code of ENOSYS, this is
	 * treated as a permanent failure with error code EOPNOTSUPP, i.e. all
	 * future removexattr() requests will fail with EOPNOTSUPP without being
	 * send to the filesystem process.
	 *
	 * Valid replies:
	 *   fuse_reply_err
	 *
	 * @param req request handle
	 * @param ino the inode number
	 * @param name of the extended attribute
	 */
	void (*removexattr) (fuse_req_t req, fuse_ino_t ino, const char *name);

	/**
	 * Check file access permissions
	 *
	 * This will be called for the access() system call.  If the
	 * 'default_permissions' mount option is given, this method is not
	 * called.
	 *
	 * This method is not called under Linux kernel versions 2.4.x
	 *
	 * If this request is answered with an error code of ENOSYS, this is
	 * treated as a permanent success, i.e. this and all future access()
	 * requests will succeed without being send to the filesystem process.
	 *
	 * Valid replies:
	 *   fuse_reply_err
	 *
	 * @param req request handle
	 * @param ino the inode number
	 * @param mask requested access mode
	 */
	void (*access) (fuse_req_t req, fuse_ino_t ino, int mask);

	/**
	 * Create and open a file
	 *
	 * If the file does not exist, first create it with the specified
	 * mode, and then open it.
	 *
	 * Open flags (with the exception of O_NOCTTY) are available in
	 * fi->flags.
	 *
	 * Filesystem may store an arbitrary file handle (pointer, index,
	 * etc) in fi->fh, and use this in other all other file operations
	 * (read, write, flush, release, fsync).
	 *
	 * There are also some flags (direct_io, keep_cache) which the
	 * filesystem may set in fi, to change the way the file is opened.
	 * See fuse_file_info structure in <fuse_common.h> for more details.
	 *
	 * If this method is not implemented or under Linux kernel
	 * versions earlier than 2.6.15, the mknod() and open() methods
	 * will be called instead.
	 *
	 * If this request is answered with an error code of ENOSYS, the handler
	 * is treated as not implemented (i.e., for this and future requests the
	 * mknod() and open() handlers will be called instead).
	 *
	 * Valid replies:
	 *   fuse_reply_create
	 *   fuse_reply_err
	 *
	 * @param req request handle
	 * @param parent inode number of the parent directory
	 * @param name to create
	 * @param mode file type and mode with which to create the new file
	 * @param fi file information
	 */
	void (*create) (fuse_req_t req, fuse_ino_t parent, const char *name,
			mode_t mode, struct fuse_file_info *fi);

	/**
	 * Test for a POSIX file lock
	 *
	 * Valid replies:
	 *   fuse_reply_lock
	 *   fuse_reply_err
	 *
	 * @param req request handle
	 * @param ino the inode number
	 * @param fi file information
	 * @param lock the region/type to test
	 */
	void (*getlk) (fuse_req_t req, fuse_ino_t ino,
		       struct fuse_file_inf