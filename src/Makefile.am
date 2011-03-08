## Process this file with automake to produce Makefile.in

INCLUDES = \
	-I$(top_builddir)/src -I$(top_srcdir)/src		\
	-DPACKAGE_LIBEXEC_DIR=\""$(libexecdir)"\"		\
	-DPACKAGE_SYSCONF_DIR=\""$(sysconfdir)"\"		\
	-DPACKAGE_DATA_DIR=\""$(datadir)"\"			\
	-DPACKAGE_BIN_DIR=\""$(bindir)"\"			\
	-DPACKAGE_LOCALSTATE_DIR=\""$(localstatedir)"\"		\
	-DPACKAGE_LOCALE_DIR=\""$(localedir)"\"			\
	-DPACKAGE_LIB_DIR=\""$(libdir)"\"			\
	-D_POSIX_PTHREAD_SEMANTICS -D_REENTRANT			\
	-I$(top_srcdir)						\
	$(GTK2_CFLAGS)						\
	$(GOBJECT_CFLAGS)					\
	$(WEBKIT_CFLAGS)					\
	$(GLIB_CFLAGS)

libexec_PROGRAMS = gourmap

gourmap_SOURCES =						\
	gourmap-ui.h						\
	gourmap-ui.c						\
	gourmap-main.c

gourmap_LDADD =							\
	$(GTK2_LIBS)						\
	$(GOBJECT_LIBS)						\
	$(WEBKIT_LIBS)						\
	$(GLIB_LIBS)

clean-local :
	rm -f *~

