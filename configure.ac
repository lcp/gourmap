AC_PREREQ(2.53)
AC_INIT(gourmap, 0.0.1)
AC_CONFIG_MACRO_DIR([m4])

AM_INIT_AUTOMAKE([foreign -Wno-portability])
AM_CONFIG_HEADER(config.h)

AC_CANONICAL_HOST

AC_GNU_SOURCE
AC_PROG_CC
AC_PROG_CC_STDC
AC_PROG_INSTALL
AC_PROG_LIBTOOL
AC_ISC_POSIX
LT_INIT
AC_HEADER_STDC
AM_PROG_CC_C_O

IT_PROG_INTLTOOL([0.40], [no-xml])
GETTEXT_PACKAGE=gourmap
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE, "$GETTEXT_PACKAGE", [The name of the gettext package.])
AC_SUBST(GETTEXT_PACKAGE)
AM_GLIB_GNU_GETTEXT

GTK_DOC_CHECK([1.9])

# TODO: bit nasty, should we use gnome-common?
CFLAGS="$CFLAGS -Wall"

# Libtool versioning
# See for details: http://bit.ly/Y5oX

# Dependencies
PKG_CHECK_MODULES(GLIB, glib-2.0 >= 2.14)
PKG_CHECK_MODULES(GOBJECT, gobject-2.0 >= 2.14)
PKG_CHECK_MODULES(WEBKIT, webkit-1.0)
PKG_CHECK_MODULES(GTK2, gtk+-2.0)

AC_OUTPUT([
        Makefile
        src/Makefile
	po/Makefile.in
])
