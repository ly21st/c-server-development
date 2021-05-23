/*
  FUSE fioclient: FUSE ioctl example client
  Copyright (C) 2008       SUSE Linux Products GmbH
  Copyright (C) 2008       Tejun Heo <teheo@suse.de>

  This program can be distributed under the terms of the GNU GPL.
  See the file COPYING.
*/

/** @file
 *
 * This program tests the cuse.c example file system.
 *
 * Example usage (assuming that /dev/foobar is a CUSE device provided
 * by the cuse.c example file system):
 *
 *     $ cuse_client /dev/foobar s
 *     0
 *
 *     $ echo "hello" | cuse_client /dev/foobar w 6
 *     Writing 6 bytes
 *     transferred 6 bytes (0 -> 6)
 *
 *     $ cuse_client /dev/foobar s
 *     6
 *
 *     $ cuse_client /dev/foobar r 10
 *     hello
 *     transferred 6 bytes (6 -> 6)
 *
 * Compiling this example
 *
 *     gcc -Wall cuse_client.c -o cuse_client
 *
 * ## Source Code ##
 * \include cuse_client.c
 */


#include <config.h>

#include <sys/types.h>
#include <sys/fcntl.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include "ioctl.h"

const char *usage =
"Usage: cuse_client FIOC_FILE COMMAND\n"
"\n"
"COMMANDS\n"
"  s [SIZE]     : get size if SIZE is omitted, set size otherwise\n"
"  r SIZE [OFF] : read SIZE bytes @ OFF (dfl 0) and output to stdout\n"
"  w SIZE [OFF] : write SIZE bytes @ OFF (dfl 0) from stdin\n"
"\n";

static int do_rw(int fd, int is_read, size_t size, off_t offset,
		 size_t *prev_size, size_t *new_size)
{
	struct fioc_rw_arg arg = { .offset = offset };
	ssize_t ret;

	arg.buf = calloc(1, size);
	if (!arg.buf) {
		fprintf(stderr, "failed to allocated %zu bytes\n", size);
		return -1;
	}

	if (is_read) {
		arg.size = size;
		ret = ioctl(fd, FIOC_READ, &arg);
		if (ret >= 0)
			fwrite(arg.buf, 1, ret, stdout);
	} else {
		arg.size = fread(arg.buf, 1, size, stdin);
		fprintf(stderr, "Writing %zu bytes\n", arg.size);
		ret = ioctl(fd, FIOC_WRITE, &arg);
	}

	if (ret >= 0) {
		*prev_size = arg.prev_size;
		*new_size = arg.new_size;
	} else
		perror("ioctl");

	free(arg.buf);
	return ret;
}

int main(int argc, char **argv)
{
	size_t param[2] = { };
	size_t size, prev_size = 0, new_size = 0;
	char cmd;
	int fd, i, rc;

	if (argc < 3)
		goto usage;

	fd = open(argv[1], O_RDWR);
	if (fd < 0) {
		perror("open");
		return 1;
	}

	cmd = tolower(argv[2][0]);
	argc -= 3;
	argv += 3;

	for (i = 0; i < argc; i++) {
		char *endp;
		param[i] = strtoul(argv[i], &endp, 0);
		if (endp == argv[i] || *endp != '\0')
			goto usage;
	}

	switch (cmd) {
	case 's':
		if (!argc) {
			if (ioctl(fd, FIOC_GET_SIZE, &size)) {
				perror("ioctl");
				return 1;
			}
			printf("%zu\n", size);
		} else {
			size = param[0];
			if (ioctl(fd, FIOC_SET_SIZE, &size)) {
				perror("ioctl");
				return 1;
			}
		}
		return 0;

	case 'r':
	case 'w':
		rc = do_rw(fd, cmd == 'r', param[0], param[1],
			   &prev_size, &new_size);
		if (rc < 0)
			return 1;
		fprintf(stderr, "transferred %d bytes (%zu -> %zu)\n",
			rc, prev_size, new_size);
		return 0;
	}

 usage:
	fprintf(stderr, "%s", usage);
	return 1;
}
m4_deps) $(CONFIGURE_DEPENDENCIES) \
	$(ACLOCAL_M4)
mkinstalldirs = $(install_sh) -d
CONFIG_HEADER = config.h
CONFIG_CLEAN_FILES =
CONFIG_CLEAN_VPATH_FILES =
AM_V_P = $(am__v_P_@AM_V@)
am__v_P_ = $(am__v_P_@AM_DEFAULT_V@)
am__v_P_0 = false
am__v_P_1 = :
AM_V_GEN = $(am__v_GEN_@AM_V@)
am__v_GEN_ = $(am__v_GEN_@AM_DEFAULT_V@)
am__v_GEN_0 = @echo "  GEN     " $@;
am__v_GEN_1 = 
AM_V_at = $(am__v_at_@AM_V@)
am__v_at_ = $(am__v_at_@AM_DEFAULT_V@)
am__v_at_0 = @
am__v_at_1 = 
SOURCES =
DIST_SOURCES =
am__can_run_installinfo = \
  case $$AM_UPDATE_INFO_DIR in \
    n|no|NO) false;; \
    *) (install-info --version) >/dev/null 2>&1;; \
  esac
am__vpath_adj_setup = srcdirstrip=`echo "$(srcdir)" | sed 's|.|.|g'`;
am__vpath_adj = case $$p in \
    $(srcdir)/*) f=`echo "$$p" | sed "s|^$$srcdirstrip/||"`;; \
    *) f=$$p;; \
  esac;
am__strip_dir = f=`echo $$p | sed -e 's|^.*/||'`;
am__install_max = 40
am__nobase_strip_setup = \
  srcdirstrip=`echo "$(srcdir)" | sed 's/[].[^$$\\*|]/\\\\&/g'`
am__nobase_strip = \
  for p in $$list; do echo "$$p"; done | sed -e "s|$$srcdirstrip/||"
am__nobase_list = $(am__nobase_strip_setup); \
  for p in $$list; do echo "$$p $$p"; done | \
  sed "s| $$srcdirstrip/| |;"' / .*\//!s/ .*/ ./; s,\( .*\)/[^/]*$$,\1,' | \
  $(AWK) 'BEGIN { files["."] = "" } { files[$$2] = files[$$2] " " $$1; \
    if (++n[$$2] == $(am__install_max)) \
      { print $$2, files[$$2]; n[$$2] = 0; files[$$2] = "" } } \
    END { for (dir in files) print dir, files[dir] }'
am__base_list = \
  sed '$$!N;$$!N;$$!N;$$!N;$$!N;$$!N;$$!N;s/\n/ /g' | \
  sed '$$!N;$$!N;$$!N;$$!N;s/\n/ /g'
am__uninstall_files_from_dir = { \
  test -z "$$files" \
    || { test ! -d "$$dir" && test ! -f "$$dir" && test ! -r "$$dir"; } \
    || { echo " ( cd '$$dir' && rm -f" $$files ")"; \
         $(am__cd) "$$dir" && rm -f $$files; }; \
  }
am__installdirs = "$(DESTDIR)$(fuseincludedir)"
HEADERS = $(fuseinclude_HEADERS) $(noinst_HEADERS)
am__tagged_files = $(HEADERS) $(SOURCES) $(TAGS_FILES) \
	$(LISP)config.h.in
# Read a list of newline-separated strings from the standard input,
# and print each of them once, without duplicates.  Input order is
# *not* preserved.
am__uniquify_input = $(AWK) '\
  BEGIN { nonempty = 0; } \
  { items[$$0] = 1; nonempty = 1; } \
  END { if (nonempty) { for (i in items) print i; }; } \
'
# Make sure the list of sources is unique.  This is necessary because,
# e.g., the same source file might be shared among _SOURCES variables
# for different programs/libraries.
am__define_uniq_tagged_files = \
  list='$(am__tagged_files)'; \
  unique=`for i in $$list; do \
    if test -f "$$i"; then echo $$i; else echo $(srcdir)/$$i; fi; \
  done | $(am__uniquify_input)`
ETAGS = etags
CTAGS = ctags
DISTFILES = $(DIST_COMMON) $(DIST_SOURCES) $(TEXINFOS) $(EXTRA_DIST)
ACLOCAL = @ACLOCAL@
AMTAR = @AMTAR@
AM_DEFAULT_VERBOSITY = @AM_DEFAULT_VERBOSITY@
AR = @AR@
AUTOCONF = @AUTOCONF@
AUTOHEADER = @AUTOHEADER@
AUTOMAKE = @AUTOMAKE@
AWK = @AWK@
CC = @CC@
CCDEPMODE = @CCDEPMODE@
CFLAGS = @CFLAGS@
CPP = @CPP@
CPPFLAGS = @CPPFLAGS@
CYGPATH_W = @CYGPATH_W@
DEFS = @DEFS@
DEPDIR = @DEPDIR@
DLLTOOL = @DLLTOOL@
DSYMUTIL = @DSYMUTIL@
DUMPBIN = @DUMPBIN@
ECHO_C = @ECHO_C@
ECHO_N = @ECHO_N@
ECHO_T = @ECHO_T@
EGREP = @EGREP@
EXEEXT = @EXEEXT@
FGREP = @FGREP@
GREP = @GREP@
INIT_D_PATH = @INIT_D_PATH@
INSTALL = @INSTALL@
INSTALL_DATA = @INSTALL_DATA@
INSTALL_PROGRAM = @INSTALL_PROGRAM@
INSTALL_SCRIPT = @INSTALL_SCRIPT@
INSTALL_STRIP_PROGRAM = @INSTALL_STRIP_PROGRAM@
LD = @LD@
LDFLAGS = @LDFLAGS@
LIBICONV = @LIBICONV@
LIBOBJS = @LIBOBJS@
LIBS = @LIBS@
LIBTOOL = @LIBTOOL@
LIPO = @LIPO@
LN_S = @LN_S@
LTLIBICONV = @LTLIBICONV@
LTLIBOBJS = @LTLIBOBJS@
MAKEINFO = @MAKEINFO@
MANIFEST_TOOL = @MANIFEST_TOOL@
MKDIR_P = @MKDIR_P@
MOUNT_FUSE_PATH = @MOUNT_FUSE_PATH@
NM = @NM@
NMEDIT = @NMEDIT@
OBJDUMP = @OBJDUMP@
OBJEXT = @OBJEXT@
OTOOL = @OTOOL@
OTOOL64 = @OTOOL64@
PACKAGE = @PACKAGE@
PACKAGE_BUGREPORT = @PACKAGE_BUGREPORT@
PACKAGE_NAME = @PACKAGE_NAME@
PACKAGE_STRING = @PACKAGE_STRING@
PACKAGE_TARNAME = @PACKAGE_TARNAME@
PACKAGE_URL = @PACKAGE_URL@
PACKAGE_VERSION = @PACKAGE_VERSION@
PATH_SEPARATOR = @PATH_SEPARATOR@
RANLIB = @RANLIB@
SED = @SED@
SET_MAKE = @SET_MAKE@
SHELL = @SHELL@
STRIP = @STRIP@
UDEV_RULES_PATH = @UDEV_RULES_PATH@
VERSION = @VERSION@
abs_builddir = @abs_builddir@
abs_srcdir = @abs_srcdir@
abs_top_builddir = @abs_top_builddir@
abs_top_srcdir = @abs_top_srcdir@
ac_ct_AR = @ac_ct_AR@
ac_ct_CC = @ac_ct_CC@
ac_ct_DUMPBIN = @ac_ct_DUMPBIN@
am__include = @am__include@
am__leading_dot = @am__leading_dot@
am__quote = @am__quote@
am__tar = @am__tar@
am__untar = @am__untar@
bindir = @bindir@
build = @build@
build_alias = @build_alias@
build_cpu = @build_cpu@
build_os = @build_os@
build_vendor = @build_vendor@
builddir = @builddir@
datadir = @datadir@
datarootdir = @datarootdir@
docdir = @docdir@
dvidir = @dvidir@
exec_prefix = @exec_prefix@
host = @host@
host_alias = @host_alias@
host_cpu = @host_cpu@
host_os = @host_os@
host_vendor = @host_vendor@
htmldir = @htmldir@
includedir = @includedir@
infodir = @infodir@
install_sh = @install_sh@
libdir = @libdir@
libexecdir = @libexecdir@
libfuse_libs = @libfuse_libs@
localedir = @localedir@
localstatedir = @localstatedir@
mandir = @mandir@
mkdir_p = @mkdir_p@
oldincludedir = @oldincludedir@
passthrough_fh_libs = @passthrough_fh_libs@
pdfdir = @pdfdir@
pkgconfigdir = @pkgconfigdir@
prefix = @prefix@
program_transform_name = @program_transform_name@
psdir = @psdir@
sbindir = @sbindir@
sharedstatedir = @sharedstatedir@
srcdir = @srcdir@
subdirs2 = @subdirs2@
sysconfdir = @sysconfdir@
target = @target@
target_alias = @target_alias@
target_cpu = @target_cpu@
target_os = @target_os@
target_vendor = @target_vendor@
top_build_prefix = @top_build_prefix@
top_builddir = @top_builddir@
top_srcdir = @top_srcdir@
fuseincludedir = $(includedir)/fuse3
fuseinclude_HEADERS = \
	fuse.h			\
	fuse_common.h		\
	fuse_lowlevel.h		\
	fuse_opt.h		\
	cuse_lowlevel.h

noinst_HEADERS = fuse_kernel.h
all: config.h
	$(MAKE) $(AM_MAKEFLAGS) all-am

.SUFFIXES:
$(srcdir)/Makefile.in:  $(srcdir)/Makefile.am  $(am__configure_deps)
	@for dep in $?; do \
	  case '$(am__configure_deps)' in \
	    *$$dep*) \
	      ( cd $(top_builddir) && $(MAKE) $(AM_MAKEFLAGS) am--refresh ) \
	        && { if test -f $@; then exit 0; else break; fi; }; \
	      exit 1;; \
	  esac; \
	done; \
	echo ' cd $(top_srcdir) && $(AUTOMAKE) --foreign include/Makefile'; \
	$(am__cd) $(top_srcdir) && \
	  $(AUTOMAKE) --foreign include/Makefile
.PRECIOUS: Makefile
Makefile: $(srcdir)/Makefile.in $(top_builddir)/config.status
	@case '$?' in \
	  *config.status*) \
	    cd $(top_builddir) && $(MAKE) $(AM_MAKEFLAGS) am--refresh;; \
	  *) \
	    echo ' cd $(top_builddir) && $(SHELL) ./config.status $(subdir)/$@ $(am__depfiles_maybe)'; \
	    cd $(top_builddir) && $(SHELL) ./config.status $(subdir)/$@ $(am__depfiles_maybe);; \
	esac;

$(top_builddir)/config.status: $(top_srcdir)/configure $(CONFIG_STATUS_DEPENDENCIES)
	cd $(top_builddir) && $(MAKE) $(AM_MAKEFLAGS) am--refresh

$(top_srcdir)/configure:  $(am__configure_deps)
	cd $(top_builddir) && $(MAKE) $(AM_MAKEFLAGS) am--refresh
$(ACLOCAL_M4):  $(am__aclocal_m4_deps)
	cd $(top_builddir) && $(MAKE) $(AM_MAKEFLAGS) am--refresh
$(am__aclocal_m4_deps):

config.h: stamp-h1
	@test -f $@ || rm -f stamp-h1
	@test -f $@ || $(MAKE) $(AM_MAKEFLAGS) stamp-h1

stamp-h1: $(srcdir)/config.h.in $(top_builddir)/config.status
	@rm -f stamp-h1
	cd $(top_builddir) && $(SHELL) ./config.status include/config.h
$(srcdir)/config.h.in:  $(am__configure_deps) 
	($(am__cd) $(top_srcdir) && $(AUTOHEADER))
	rm -f stamp-h1
	touch $@

distclean-hdr:
	-rm -f config.h stamp-h1

mostlyclean-libtool:
	-rm -f *.lo

clean-libtool:
	-rm -rf .libs _libs
install-fuseincludeHEADERS: $(fuseinclude_HEADERS)
	@$(NORMAL_INSTALL)
	@list='$(fuseinclude_HEADERS)'; test -n "$(fuseincludedir)" || list=; \
	if test -n "$$list"; then \
	  echo " $(MKDIR_P) '$(DESTDIR)$(fuseincludedir)'"; \
	  $(MKDIR_P) "$(DESTDIR)$(fuseincludedir)" || exit 1; \
	fi; \
	for p in $$list; do \
	  if test -f "$$p"; then d=; else d="$(srcdir)/"; fi; \
	  echo "$$d$$p"; \
	done | $(am__base_list) | \
	while read files; do \
	  echo " $(INSTALL_HEADER) $$files '$(DESTDIR)$(fuseincludedir)'"; \
	  $(INSTALL_HEADER) $$files "$(DESTDIR)$(fuseincludedir)" || exit $$?; \
	done

uninstall-fuseincludeHEADERS:
	@$(NORMAL_UNINSTALL)
	@list='$(fuseinclude_HEADERS)'; test -n "$(fuseincludedir)" || list=; \
	files=`for p in $$list; do echo $$p; done | sed -e 's|^.*/||'`; \
	dir='$(DESTDIR)$(fuseincludedir)'; $(am__uninstall_files_from_dir)

ID: $(am__tagged_files)
	$(am__define_uniq_tagged_files); mkid -fID $$unique
tags: tags-am
TAGS: tags

tags-am: $(TAGS_DEPENDENCIES) $(am__tagged_files)
	set x; \
	here=`pwd`; \
	$(am__define_uniq_tagged_files); \
	shift; \
	if test -z "$(ETAGS_ARGS)$$*$$unique"; then :; else \
	  test -n "$$unique" || unique=$$empty_fix; \
	  if test $$# -gt 0; then \
	    $(ETAGS) $(ETAGSFLAGS) $(AM_ETAGSFLAGS) $(ETAGS_ARGS) \
	      "$$@" $$unique; \
	  else \
	    $(ETAGS) $(ETAGSFLAGS) $(AM_ETAGSFLAGS) $(ETAGS_ARGS) \
	      $$unique; \
	  fi; \
	fi
ctags: ctags-am

CTAGS: ctags
ctags-am: $(TAGS_DEPENDENCIES) $(am__tagged_files)
	$(am__define_uniq_tagged_files); \
	test -z "$(CTAGS_ARGS)$$unique" \
	  || $(CTAGS) $(CTAGSFLAGS) $(AM_CTAGSFLAGS) $(CTAGS_ARGS) \
	     $$unique

GTAGS:
	here=`$(am__cd) $(top_builddir) && pwd` \
	  && $(am__cd) $(top_srcdir) \
	  && gtags -i $(GTAGS_ARGS) "$$here"
cscopelist: cscopelist-am

cscopelist-am: $(am__tagged_files)
	list='$(am__tagged_files)'; \
	case "$(srcdir)" in \
	  [\\/]* | ?:[\\/]*) sdir="$(srcdir)" ;; \
	  *) sdir=$(subdir)/$(srcdir) ;; \
	esac; \
	for i in $$list; do \
	  if test -f "$$i"; then \
	    echo "$(subdir)/$$i"; \
	  else \
	    echo "$$sdir/$$i"; \
	  fi; \
	done >> $(top_builddir)/cscope.files

distclean-tags:
	-rm -f TAGS ID GTAGS GRTAGS GSYMS GPATH tags

distdir: $(DISTFILES)
	@srcdirstrip=`echo "$(srcdir)" | sed 's/[].[^$$\\*]/\\\\&/g'`; \
	topsrcdirstrip=`echo "$(top_srcdir)" | sed 's/[].[^$$\\*]/\\\\&/g'`; \
	list='$(DISTFILES)'; \
	  dist_files=`for file in $$list; do echo $$file; done | \
	  sed -e "s|^$$srcdirstrip/||;t" \
	      -e "s|^$$topsrcdirstrip/|$(top_builddir)/|;t"`; \
	case $$dist_files in \
	  */*) $(MKDIR_P) `echo "$$dist_files" | \
			   sed '/\//!d;s|^|$(distdir)/|;s,/[^/]*$$,,' | \
			   sort -u` ;; \
	esac; \
	for file in $$dist_files; do \
	  if test -f $$file || test -d $$file; then d=.; else d=$(srcdir); fi; \
	  if test -d $$d/$$file; then \
	    dir=`echo "/$$file" | sed -e 's,/[^/]*$$,,'`; \
	    if test -d "$(distdir)/$$file"; then \
	      find "$(distdir)/$$file" -type d ! -perm -700 -exec chmod u+rwx {} \;; \
	    fi; \
	    if test -d $(srcdir)/$$file && test $$d != $(srcdir); then \
	      cp -fpR $(srcdir)/$$file "$(distdir)$$dir" || exit 1; \
	      find "$(distdir)/$$file" -type d ! -perm -700 -exec chmod u+rwx {} \;; \
	    fi; \
	    cp -fpR $$d/$$file "$(distdir)$$dir" || exit 1; \
	  else \
	    test -f "$(distdir)/$$file" \
	    || cp -p $$d/$$file "$(distdir)/$$file" \
	    || exit 1; \
	  fi; \
	done
check-am: all-am
check: check-am
all-am: Makefile $(HEADERS) config.h
installdirs:
	for dir in "$(DESTDIR)$(fuseincludedir)"; do \
	  test -z "$$dir" || $(MKDIR_P) "$$dir"; \
	done
install: install-am
install-exec: install-exec-am
install-data: install-data-am
uninstall: uninstall-am

install-am: all-am
	@$(MAKE) $(AM_MAKEFLAGS) install-exec-am install-data-am

installcheck: installcheck-am
install-strip:
	if test -z '$(STRIP)'; then \
	  $(MAKE) $(AM_MAKEFLAGS) INSTALL_PROGRAM="$(INSTALL_STRIP_PROGRAM)" \
	    install_sh_PROGRAM="$(INSTALL_STRIP_PROGRAM)" INSTALL_STRIP_FLAG=-s \
	      install; \
	else \
	  $(MAKE) $(AM_MAKEFLAGS) INSTALL_PROGRAM="$(INSTALL_STRIP_PROGRAM)" \
	    install_sh_PROGRAM="$(INSTALL_STRIP_PROGRAM)" INSTALL_STRIP_FLAG=-s \
	    "INSTALL_PROGRAM_ENV=STRIPPROG='$(STRIP)'" install; \
	fi
mostlyclean-generic:

clean-generic:

distclean-generic:
	-test -z "$(CONFIG_CLEAN_FILES)" || rm -f $(CONFIG_CLEAN_FILES)
	-test . = "$(srcdir)" || test -z "$(CONFIG_CLEAN_VPATH_FILES)" || rm -f $(CONFIG_CLEAN_VPATH_FILES)

maintainer-clean-generic:
	@echo "This command is intended for maintainers to use"
	@echo "it deletes files that may require special tools to rebuild."
clean: clean-am

clean-am: clean-generic clean-libtool mostlyclean-am

distclean: distclean-am
	-rm -f Makefile
distclean-am: clean-am distclean-generic distclean-hdr distclean-tags

dvi: dvi-am

dvi-am:

html: html-am

html-am:

info: info-am

info-am:

install-data-am: install-fuseincludeHEADERS

install-dvi: install-dvi-am

install-dvi-am:

install-exec-am:

install-html: install-html-am

install-html-am:

install-info: install-info-am

install-info-am:

install-man:

install-pdf: install-pdf-am

install-pdf-am:

install-ps: install-ps-am

install-ps-am:

installcheck-am:

maintainer-clean: maintainer-clean-am
	-rm -f Makefile
maintainer-clean-am: distclean-am maintainer-clean-generic

mostlyclean: mostlyclean-am

mostlyclean-am: mostlyclean-generic mostlyclean-libtool

pdf: pdf-am

pdf-am:

ps: ps-am

ps-am:

uninstall-am: uninstall-fuseincludeHEADERS

.MAKE: all install-am install-strip

.PHONY: CTAGS GTAGS TAGS all all-am check check-am clean clean-generic \
	clean-libtool cscopelist-am ctags ctags-am distclean \
	distclean-generic distclean-hdr distclean-libtool \
	distclean-tags distdir dvi dvi-am html html-am info info-am \
	install install-am install-data install-data-am install-dvi \
	install-dvi-am install-exec install-exec-am \
	install-fuseincludeHEADERS install-html install-html-am \
	install-info install-info-am install-man install-pdf \
	install-pdf-am install-ps install-ps-am install-strip \
	installcheck installcheck-am installdirs maintainer-clean \
	maintainer-clean-generic mostlyclean mostlyclean-generic \
	mostlyclean-libtool pdf pdf-am ps ps-am tags tags-am uninstall \
	uninstall-am uninstall-fuseincludeHEADERS


# Tell versions [3.59,3.63) of GNU make to not export all variables.
# Otherwise a system limit (for SysV at least) may be exceeded.
.NOEXPORT:
