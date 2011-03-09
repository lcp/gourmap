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
	$(REST_CFLAGS)						\
	$(JSON_GLIB_CFLAGS)					\
	$(GTK2_CFLAGS)						\
	$(GOBJECT_CFLAGS)					\
	$(WEBKIT_CFLAGS)					\
	$(GLIB_CFLAGS)

libexec_PROGRAMS = gourmap

gourmap_SOURCES =						\
	gourmap-util.h						\
	gourmap-util.c						\
	gourmap-poi.h						\
	gourmap-poi.c						\
	gourmap-ui.h						\
	gourmap-ui.c						\
	gourmap-main.c

gourmap_LDADD =							\
	$(REST_LIBS)						\
	$(JSON_GLIB_LIBS)					\
	$(GTK2_LIBS)						\
	$(GOBJECT_LIBS)						\
	$(WEBKIT_LIBS)						\
	$(GLIB_LIBS)						\
	-lm

clean-local :
	rm -f *~

