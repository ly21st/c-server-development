/*
  FUSE: Filesystem in Userspace
  Copyright (C) 2001-2007  Miklos Szeredi <miklos@szeredi.hu>

  This program can be distributed under the terms of the GNU GPL.
  See the file COPYING.
*/

#include <config.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

static char *progname;

static char *xstrdup(const char *s)
{
	char *t = strdup(s);
	if (!t) {
		fprintf(stderr, "%s: failed to allocate memory\n", progname);
		exit(1);
	}
	return t;
}

static void *xrealloc(void *oldptr, size_t size)
{
	void *ptr = realloc(oldptr, size);
	if (!ptr) {
		fprintf(stderr, "%s: failed to allocate memory\n", progname);
		exit(1);
	}
	return ptr;
}

static void add_arg(char **cmdp, const char *opt)
{
	size_t optlen = strlen(opt);
	size_t cmdlen = *cmdp ? strlen(*cmdp) : 0;
	char *cmd = xrealloc(*cmdp, cmdlen + optlen * 4 + 4);
	char *s;
	s = cmd + cmdlen;
	if (*cmdp)
		*s++ = ' ';

	*s++ = '\'';
	for (; *opt; opt++) {
		if (*opt == '\'') {
			*s++ = '\'';
			*s++ = '\\';
			*s++ = '\'';
			*s++ = '\'';
		} else
			*s++ = *opt;
	}
	*s++ = '\'';
	*s = '\0';
	*cmdp = cmd;
}

static char *add_option(const char *opt, char *options)
{
	int oldlen = options ? strlen(options) : 0;

	options = xrealloc(options, oldlen + 1 + strlen(opt) + 1);
	if (!oldlen)
		strcpy(options, opt);
	else {
		strcat(options, ",");
		strcat(options, opt);
	}
	return options;
}

int main(int argc, char *argv[])
{
	char *type = NULL;
	char *source;
	const char *mountpoint;
	char *basename;
	char *options = NULL;
	char *command = NULL;
	char *setuid = NULL;
	int i;
	int dev = 1;
	int suid = 1;

	progname = argv[0];
	basename = strrchr(argv[0], '/');
	if (basename)
		basename++;
	else
		basename = argv[0];

	if (strncmp(basename, "mount.fuse.", 11) == 0)
		type = basename + 11;
	if (strncmp(basename, "mount.fuseblk.", 14) == 0)
		type = basename + 14;

	if (type && !type[0])
		type = NULL;

	if (argc < 3) {
		fprintf(stderr,
			"usage: %s %s destination [-t type] [-o opt[,opts...]]\n",
			progname, type ? "source" : "type#[source]");
		exit(1);
	}

	source = argv[1];
	if (!source[0])
		source = NULL;

	mountpoint = argv[2];

	for (i = 3; i < argc; i++) {
		if (strcmp(argv[i], "-v") == 0) {
			continue;
		} else if (strcmp(argv[i], "-t") == 0) {
			i++;

			if (i == argc) {
				fprintf(stderr,
					"%s: missing argument to option '-t'\n",
					progname);
				exit(1);
			}
			type = argv[i];
			if (strncmp(type, "fuse.", 5) == 0)
				type += 5;
			else if (strncmp(type, "fuseblk.", 8) == 0)
				type += 8;

			if (!type[0]) {
				fprintf(stderr,
					"%s: empty type given as argument to option '-t'\n",
					progname);
				exit(1);
			}
		} else	if (strcmp(argv[i], "-o") == 0) {
			char *opts;
			char *opt;
			i++;
			if (i == argc)
				break;

			opts = xstrdup(argv[i]);
			opt = strtok(opts, ",");
			while (opt) {
				int j;
				int ignore = 0;
				const char *ignore_opts[] = { "",
							      "user",
							      "nouser",
							      "users",
							      "auto",
							      "noauto",
							      "_netdev",
							      NULL};
				if (strncmp(opt, "setuid=", 7) == 0) {
					setuid = xstrdup(opt + 7);
					ignore = 1;
				}
				for (j = 0; ignore_opts[j]; j++)
					if (strcmp(opt, ignore_opts[j]) == 0)
						ignore = 1;

				if (!ignore) {
					if (strcmp(opt, "nodev") == 0)
						dev = 0;
					else if (strcmp(opt, "nosuid") == 0)
						suid = 0;

					options = add_option(opt, options);
				}
				opt = strtok(NULL, ",");
			}
		}
	}

	if (dev)
		options = add_option("dev", options);
	if (suid)
		options = add_option("suid", options);

	if (!type) {
		if (source) {
			type = xstrdup(source);
			source = strchr(type, '#');
			if (source)
				*source++ = '\0';
			if (!type[0]) {
				fprintf(stderr, "%s: empty filesystem type\n",
					progname);
				exit(1);
			}
		} else {
			fprintf(stderr, "%s: empty source\n", progname);
			exit(1);
		}
	}

	add_arg(&command, type);
	if (source)
		add_arg(&command, source);
	add_arg(&command, mountpoint);
	if (options) {
		add_arg(&command, "-o");
		add_arg(&command, options);
	}

	if (setuid && setuid[0]) {
		char *sucommand = command;
		command = NULL;
		add_arg(&command, "su");
		add_arg(&command, "-");
		add_arg(&command, setuid);
		add_arg(&command, "-c");
		add_arg(&command, sucommand);
	} else if (!getenv("HOME")) {
		/* Hack to make filesystems work in the boot environment */
		setenv("HOME", "/root", 0);
	}

	execl("/bin/sh", "/bin/sh", "-c", command, NULL);
	fprintf(stderr, "%s: failed to execute /bin/sh: %s\n", progname,
		strerror(errno));
	return 1;
}
__v_CC_0 = @echo "  CC      " $@;
am__v_CC_1 = 
CCLD = $(CC)
LINK = $(LIBTOOL) $(AM_V_lt) --tag=CC $(AM_LIBTOOLFLAGS) \
	$(LIBTOOLFLAGS) --mode=link $(CCLD) $(AM_CFLAGS) $(CFLAGS) \
	$(AM_LDFLAGS) $(LDFLAGS) -o $@
AM_V_CCLD = $(am__v_CCLD_@AM_V@)
am__v_CCLD_ = $(am__v_CCLD_@AM_DEFAULT_V@)
am__v_CCLD_0 = @echo "  CCLD    " $@;
am__v_CCLD_1 = 
SOURCES = $(fusermount3_SOURCES) $(mount_fuse3_SOURCES)
DIST_SOURCES = $(fusermount3_SOURCES) $(mount_fuse3_SOURCES)
am__can_run_installinfo = \
  case $$AM_UPDATE_INFO_DIR in \
    n|no|NO) false;; \
    *) (install-info --version) >/dev/null 2>&1;; \
  esac
am__tagged_files = $(HEADERS) $(SOURCES) $(TAGS_FILES) $(LISP)
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

# we re-use mount_util.c from the library, but do want to keep ourself
# as stand-alone as possible. in order to make an out-of-source build
# possible, we "generate" the file from its original location by
# copying it over.
fusermount3_SOURCES = fusermount.c mount_util.c
fusermount3_CPPFLAGS = -I$(top_srcdir)/lib
BUILT_SOURCES = mount_util.c
mount_fuse3_SOURCES = mount.fuse.c
EXTRA_DIST = udev.rules init_script
all: $(BUILT_SOURCES)
	$(MAKE) $(AM_MAKEFLAGS) all-am

.SUFFIXES:
.SUFFIXES: .c .lo .o .obj
$(srcdir)/Makefile.in:  $(srcdir)/Makefile.am  $(am__configure_deps)
	@for dep in $?; do \
	  case '$(am__configure_deps)' in \
	    *$$dep*) \
	      ( cd $(top_builddir) && $(MAKE) $(AM_MAKEFLAGS) am--refresh ) \
	        && { if test -f $@; then exit 0; else break; fi; }; \
	      exit 1;; \
	  esac; \
	done; \
	echo ' cd $(top_srcdir) && $(AUTOMAKE) --foreign util/Makefile'; \
	$(am__cd) $(top_srcdir) && \
	  $(AUTOMAKE) --foreign util/Makefile
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
install-binPROGRAMS: $(bin_PROGRAMS)
	@$(NORMAL_INSTALL)
	@list='$(bin_PROGRAMS)'; test -n "$(bindir)" || list=; \
	if test -n "$$list"; then \
	  echo " $(MKDIR_P) '$(DESTDIR)$(bindir)'"; \
	  $(MKDIR_P) "$(DESTDIR)$(bindir)" || exit 1; \
	fi; \
	for p in $$list; do echo "$$p $$p"; done | \
	sed 's/$(EXEEXT)$$//' | \
	while read p p1; do if test -f $$p \
	 || test -f $$p1 \
	  ; then echo "$$p"; echo "$$p"; else :; fi; \
	done | \
	sed -e 'p;s,.*/,,;n;h' \
	    -e 's|.*|.|' \
	    -e 'p;x;s,.*/,,;s/$(EXEEXT)$$//;$(transform);s/$$/$(EXEEXT)/' | \
	sed 'N;N;N;s,\n, ,g' | \
	$(AWK) 'BEGIN { files["."] = ""; dirs["."] = 1 } \
	  { d=$$3; if (dirs[d] != 1) { print "d", d; dirs[d] = 1 } \
	    if ($$2 == $$4) files[d] = files[d] " " $$1; \
	    else { print "f", $$3 "/" $$4, $$1; } } \
	  END { for (d in files) print "f", d, files[d] }' | \
	while read type dir files; do \
	    if test "$$dir" = .; then dir=; else dir=/$$dir; fi; \
	    test -z "$$files" || { \
	    echo " $(INSTALL_PROGRAM_ENV) $(LIBTOOL) $(AM_LIBTOOLFLAGS) $(LIBTOOLFLAGS) --mode=install $(INSTALL_PROGRAM) $$files '$(DESTDIR)$(bindir)$$dir'"; \
	    $(INSTALL_PROGRAM_ENV) $(LIBTOOL) $(AM_LIBTOOLFLAGS) $(LIBTOOLFLAGS) --mode=install $(INSTALL_PROGRAM) $$files "$(DESTDIR)$(bindir)$$dir" || exit $$?; \
	    } \
	; done

uninstall-binPROGRAMS:
	@$(NORMAL_UNINSTALL)
	@list='$(bin_PROGRAMS)'; test -n "$(bindir)" || list=; \
	files=`for p in $$list; do echo "$$p"; done | \
	  sed -e 'h;s,^.*/,,;s/$(EXEEXT)$$//;$(transform)' \
	      -e 's/$$/$(EXEEXT)/' \
	`; \
	test -n "$$list" || exit 0; \
	echo " ( cd '$(DESTDIR)$(bindir)' && rm -f" $$files ")"; \
	cd "$(DESTDIR)$(bindir)" && rm -f $$files

clean-binPROGRAMS:
	@list='$(bin_PROGRAMS)'; test -n "$$list" || exit 0; \
	echo " rm -f" $$list; \
	rm -f $$list || exit $$?; \
	test -n "$(EXEEXT)" || exit 0; \
	list=`for p in $$list; do echo "$$p"; done | sed 's/$(EXEEXT)$$//'`; \
	echo " rm -f" $$list; \
	rm -f $$list

clean-noinstPROGRAMS:
	@list='$(noinst_PROGRAMS)'; test -n "$$list" || exit 0; \
	echo " rm -f" $$list; \
	rm -f $$list || exit $$?; \
	test -n "$(EXEEXT)" || exit 0; \
	list=`for p in $$list; do echo "$$p"; done | sed 's/$(EXEEXT)$$//'`; \
	echo " rm -f" $$list; \
	rm -f $$list

fusermount3$(EXEEXT): $(fusermount3_OBJECTS) $(fusermount3_DEPENDENCIES) $(EXTRA_fusermount3_DEPENDENCIES) 
	@rm -f fusermount3$(EXEEXT)
	$(AM_V_CCLD)$(LINK) $(fusermount3_OBJECTS) $(fusermount3_LDADD) $(LIBS)

mount.fuse3$(EXEEXT): $(mount_fuse3_OBJECTS) $(mount_fuse3_DEPENDENCIES) $(EXTRA_mount_fuse3_DEPENDENCIES) 
	@rm -f mount.fuse3$(EXEEXT)
	$(AM_V_CCLD)$(LINK) $(mount_fuse3_OBJECTS) $(mount_fuse3_LDADD) $(LIBS)

mostlyclean-compile:
	-rm -f *.$(OBJEXT)

distclean-compile:
	-rm -f *.tab.c

@AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/fusermount3-fusermount.Po@am__quote@
@AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/fusermount3-mount_util.Po@am__quote@
@AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/mount.fuse.Po@am__quote@

.c.o:
@am__fastdepCC_TRUE@	$(AM_V_CC)depbase=`echo $@ | sed 's|[^/]*$$|$(DEPDIR)/&|;s|\.o$$||'`;\
@am__fastdepCC_TRUE@	$(COMPILE) -MT $@ -MD -MP -MF $$depbase.Tpo -c -o $@ $< &&\
@am__fastdepCC_TRUE@	$(am__mv) $$depbase.Tpo $$depbase.Po
@AMDEP_TRUE@@am__fastdepCC_FALSE@	$(AM_V_CC)source='$<' object='$@' libtool=no @AMDEPBACKSLASH@
@AMDEP_TRUE@@am__fastdepCC_FALSE@	DEPDIR=$(DEPDIR) $(CCDEPMODE) $(depcomp) @AMDEPBACKSLASH@
@am__fastdepCC_FALSE@	$(AM_V_CC@am__nodep@)$(COMPILE) -c -o $@ $<

.c.obj:
@am__fastdepCC_TRUE@	$(AM_V_CC)depbase=`echo $@ | sed 's|[^/]*$$|$(DEPDIR)/&|;s|\.obj$$||'`;\
@am__fastdepCC_TRUE@	$(COMPILE) -MT $@ -MD -MP -MF $$depbase.Tpo -c -o $@ `$(CYGPATH_W) '$<'` &&\
@am__fastdepCC_TRUE@	$(am__mv) $$depbase.Tpo $$depbase.Po
@AMDEP_TRUE@@am__fastdepCC_FALSE@	$(AM_V_CC)source='$<' object='$@' libtool=no @AMDEPBACKSLASH@
@AMDEP_TRUE@@am__fastdepCC_FALSE@	DEPDIR=$(DEPDIR) $(CCDEPMODE) $(depcomp) @AMDEPBACKSLASH@
@am__fastdepCC_FALSE@	$(AM_V_CC@am__nodep@)$(COMPILE) -c -o $@ `$(CYGPATH_W) '$<'`

.c.lo:
@am__fastdepCC_TRUE@	$(AM_V_CC)depbase=`echo $@ | sed 's|[^/]*$$|$(DEPDIR)/&|;s|\.lo$$||'`;\
@am__fastdepCC_TRUE@	$(LTCOMPILE) -MT $@ -MD -MP -MF $$depbase.Tpo -c -o $@ $< &&\
@am__fastdepCC_TRUE@	$(am__mv) $$depbase.Tpo $$depbase.Plo
@AMDEP_TRUE@@am__fastdepCC_FALSE@	$(AM_V_CC)source='$<' object='$@' libtool=yes @AMDEPBACKSLASH@
@AMDEP_TRUE@@am__fastdepCC_FALSE@	DEPDIR=$(DEPDIR) $(CCDEPMODE) $(depcomp) @AMDEPBACKSLASH@
@am__fastdepCC_FALSE@	$(AM_V_CC@am__nodep@)$(LTCOMPILE) -c -o $@ $<

fusermount3-fusermount.o: fusermount.c
@am__fastdepCC_TRUE@	$(AM_V_CC)$(CC) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(fusermount3_CPPFLAGS) $(CPPFLAGS) $(AM_CFLAGS) $(CFLAGS) -MT fusermount3-fusermount.o -MD -MP -MF $(DEPDIR)/fusermount3-fusermount.Tpo -c -o fusermount3-fusermount.o `test -f 'fusermount.c' || echo '$(srcdir)/'`fusermount.c
@am__fastdepCC_TRUE@	$(AM_V_at)$(am__mv) $(DEPDIR)/fusermount3-fusermount.Tpo $(DEPDIR)/fusermount3-fusermount.Po
@AMDEP_TRUE@@am__fastdepCC_FALSE@	$(AM_V_CC)source='fusermount.c' object='fusermount3-fusermount.o' libtool=no @AMDEPBACKSLASH@
@AMDEP_TRUE@@am__fastdepCC_FALSE@	DEPDIR=$(DEPDIR) $(CCDEPMODE) $(depcomp) @AMDEPBACKSLASH@
@am__fastdepCC_FALSE@	$(AM_V_CC@am__nodep@)$(CC) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(fusermount3_CPPFLAGS) $(CPPFLAGS) $(AM_CFLAGS) $(CFLAGS) -c -o fusermount3-fusermount.o `test -f 'fusermount.c' || echo '$(srcdir)/'`fusermount.c

fusermount3-fusermount.obj: fusermount.c
@am__fastdepCC_TRUE@	$(AM_V_CC)$(CC) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(fusermount3_CPPFLAGS) $(CPPFLAGS) $(AM_CFLAGS) $(CFLAGS) -MT fusermount3-fusermount.obj -MD -MP -MF $(DEPDIR)/fusermount3-fusermount.Tpo -c -o fusermount3-fusermount.obj `if test -f 'fusermount.c'; then $(CYGPATH_W) 'fusermount.c'; else $(CYGPATH_W) '$(srcdir)/fusermount.c'; fi`
@am__fastdepCC_TRUE@	$(AM_V_at)$(am__mv) $(DEPDIR)/fusermount3-fusermount.Tpo $(DEPDIR)/fusermount3-fusermount.Po
@AMDEP_TRUE@@am__fastdepCC_FALSE@	$(AM_V_CC)source='fusermount.c' object='fusermount3-fusermount.obj' libtool=no @AMDEPBACKSLASH@
@AMDEP_TRUE@@am__fastdepCC_FALSE@	DEPDIR=$(DEPDIR) $(CCDEPMODE) $(depcomp) @AMDEPBACKSLASH@
@am__fastdepCC_FALSE@	$(AM_V_CC@am__nodep@)$(CC) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(fusermount3_CPPFLAGS) $(CPPFLAGS) $(AM_CFLAGS) $(CFLAGS) -c -o fusermount3-fusermount.obj `if test -f 'fusermount.c'; then $(CYGPATH_W) 'fusermount.c'; else $(CYGPATH_W) '$(srcdir)/fusermount.c'; fi`

fusermount3-mount_util.o: mount_util.c
@am__fastdepCC_TRUE@	$(AM_V_CC)$(CC) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(fusermount3_CPPFLAGS) $(CPPFLAGS) $(AM_CFLAGS) $(CFLAGS) -MT fusermount3-mount_util.o -MD -MP -MF $(DEPDIR)/fusermount3-mount_util.Tpo -c -o fusermount3-mount_util.o `test -f 'mount_util.c' || echo '$(srcdir)/'`mount_util.c
@am__fastdepCC_TRUE@	$(AM_V_at)$(am__mv) $(DEPDIR)/fusermount3-mount_util.Tpo $(DEPDIR)/fusermount3-mount_util.Po
@AMDEP_TRUE@@am__fastdepCC_FALSE@	$(AM_V_CC)source='mount_util.c' object='fusermount3-mount_util.o' libtool=no @AMDEPBACKSLASH@
@AMDEP_TRUE@@am__fastdepCC_FALSE@	DEPDIR=$(DEPDIR) $(CCDEPMODE) $(depcomp) @AMDEPBACKSLASH@
@am__fastdepCC_FALSE@	$(AM_V_CC@am__nodep@)$(CC) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(fusermount3_CPPFLAGS) $(CPPFLAGS) $(AM_CFLAGS) $(CFLAGS) -c -o fusermount3-mount_util.o `test -f 'mount_util.c' || echo '$(srcdir)/'`mount_util.c

fusermount3-mount_util.obj: mount_util.c
@am__fastdepCC_TRUE@	$(AM_V_CC)$(CC) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(fusermount3_CPPFLAGS) $(CPPFLAGS) $(AM_CFLAGS) $(CFLAGS) -MT fusermount3-mount_util.obj -MD -MP -MF $(DEPDIR)/fusermount3-mount_util.Tpo -c -o fusermount3-mount_util.obj `if test -f 'mount_util.c'; then $(CYGPATH_W) 'mount_util.c'; else $(CYGPATH_W) '$(srcdir)/mount_util.c'; fi`
@am__fastdepCC_TRUE@	$(AM_V_at)$(am__mv) $(DEPDIR)/fusermount3-mount_util.Tpo $(DEPDIR)/fusermount3-mount_util.Po
@AMDEP_TRUE@@am__fastdepCC_FALSE@	$(AM_V_CC)source='mount_util.c' object='fusermount3-mount_util.obj' libtool=no @AMDEPBACKSLASH@
@AMDEP_TRUE@@am__fastdepCC_FALSE@	DEPDIR=$(DEPDIR) $(CCDEPMODE) $(depcomp) @AMDEPBACKSLASH@
@am__fastdepCC_FALSE@	$(AM_V_CC@am__nodep@)$(CC) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(fusermount3_CPPFLAGS) $(CPPFLAGS) $(AM_CFLAGS) $(CFLAGS) -c -o fusermount3-mount_util.obj `if test -f 'mount_util.c'; then $(CYGPATH_W) 'mount_util.c'; else $(CYGPATH_W) '$(srcdir)/mount_util.c'; fi`

mostlyclean-libtool:
	-rm -f *.lo

clean-libtool:
	-rm -rf .libs _libs

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
check: $(BUILT_SOURCES)
	$(MAKE) $(AM_MAKEFLAGS) check-am
all-am: Makefile $(PROGRAMS)
installdirs:
	for dir in "$(DESTDIR)$(bindir)"; do \
	  test -z "$$dir" || $(MKDIR_P) "$$dir"; \
	done
install: $(BUILT_SOURCES)
	$(MAKE) $(AM_MAKEFLAGS) install-am
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
	-test -z "$(BUILT_SOURCES)" || rm -f $(BUILT_SOURCES)
clean: clean-am

clean-am: clean-binPROGRAMS clean-generic clean-libtool \
	clean-noinstPROGRAMS mostlyclean-am

distclean: distclean-am
	-rm -rf ./$(DEPDIR)
	-rm -f Makefile
distclean-am: clean-am distclean-compile distclean-generic \
	distclean-tags

dvi: dvi-am

dvi-am:

html: html-am

html-am:

info: info-am

info-am:

install-data-am: install-data-local

install-dvi: install-dvi-am

install-dvi-am:

install-exec-am: install-binPROGRAMS install-exec-local
	@$(NORMAL_INSTALL)
	$(MAKE) $(AM_MAKEFLAGS) install-exec-hook
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
	-rm -rf ./$(DEPDIR)
	-rm -f Makefile
maintainer-clean-am: distclean-am maintainer-clean-generic

mostlyclean: mostlyclean-am

mostlyclean-am: mostlyclean-compile mostlyclean-generic \
	mostlyclean-libtool

pdf: pdf-am

pdf-am:

ps: ps-am

ps-am:

uninstall-am: uninstall-binPROGRAMS uninstall-local

.MAKE: all check install install-am install-exec-am install-strip

.PHONY: CTAGS GTAGS TAGS all all-am check check-am clean \
	clean-binPROGRAMS clean-generic clean-libtool \
	clean-noinstPROGRAMS cscopelist-am ctags ctags-am distclean \
	distclean-compile distclean-generic distclean-libtool \
	distclean-tags distdir dvi dvi-am html html-am info info-am \
	install install-am install-binPROGRAMS install-data \
	install-data-am install-data-local install-dvi install-dvi-am \
	install-exec install-exec-am install-exec-hook \
	install-exec-local install-html install-html-am install-info \
	install-info-am install-man install-pdf install-pdf-am \
	install-ps install-ps-am install-strip installcheck \
	installcheck-am installdirs maintainer-clean \
	maintainer-clean-generic mostlyclean mostlyclean-compile \
	mostlyclean-generic mostlyclean-libtool pdf pdf-am ps ps-am \
	tags tags-am uninstall uninstall-am uninstall-binPROGRAMS \
	uninstall-local

mount_util.c: $(top_srcdir)/lib/mount_util.c
	@cp $(top_srcdir)/lib/mount_util.c .

install-exec-hook:
	-chmod u+s $(DESTDIR)$(bindir)/fusermount3
	@if test ! -e $(DESTDIR)/dev/fuse; then \
		$(MKDIR_P) $(DESTDIR)/dev; \
		echo "mknod $(DESTDIR)/dev/fuse -m 0666 c 10 229 || true"; \
		mknod $(DESTDIR)/dev/fuse -m 0666 c 10 229 || true; \
	fi

install-exec-local:
	$(MKDIR_P) $(DESTDIR)$(MOUNT_FUSE_PATH)
	$(INSTALL_PROGRAM) $(builddir)/mount.fuse3 $(DESTDIR)$(MOUNT_FUSE_PATH)/mount.fuse3
	$(MKDIR_P) $(DESTDIR)$(INIT_D_PATH)
	$(INSTALL_SCRIPT) $(srcdir)/init_script $(DESTDIR)$(INIT_D_PATH)/fuse3
	@if test -x /usr/sbin/update-rc.d; then \
		echo "/usr/sbin/update-rc.d fuse start 34 S . start 41 0 6 . || true"; \
		/usr/sbin/update-rc.d fuse start 34 S . start 41 0 6 . || true; \
	fi

install-data-local:
	$(MKDIR_P) $(DESTDIR)$(UDEV_RULES_PATH)
	$(INSTALL_DATA) $(srcdir)/udev.rules $(DESTDIR)$(UDEV_RULES_PATH)/99-fuse3.rules

uninstall-local:
	rm -f $(DESTDIR)$(MOUNT_FUSE_PATH)/mount.fuse3
	rm -f $(DESTDIR)$(UDEV_RULES_PATH)/99-fuse.rules
	rm -f $(DESTDIR)$(INIT_D_PATH)/fuse
	@if test -x /usr/sbin/update-rc.d; then \
		echo "/usr/sbin/update-rc.d fuse remove || true"; \
		/usr/sbin/update-rc.d fuse remove || true; \
	fi

# Tell versions [3.59,3.63) of GNU make to not export all variables.
# Otherwise a system limit (for SysV at least) may be exceeded.
.NOEXPORT:
