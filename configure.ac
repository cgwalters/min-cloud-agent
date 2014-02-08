AC_PREREQ([2.63])
AC_INIT([min-metadata-service], [2014.1], [walters@verbum.org], [min-metadata-service], [https://github.com/cgwalters/min-metadata-service])
AC_CONFIG_HEADER([config.h])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_AUX_DIR([build-aux])

AM_INIT_AUTOMAKE([1.11 -Wno-portability foreign no-define tar-ustar no-dist-gzip dist-xz])
AM_MAINTAINER_MODE([enable])
AM_SILENT_RULES([yes])

AC_PROG_CC
AM_PROG_CC_C_O

changequote(,)dnl
if test "x$GCC" = "xyes"; then
  WARN_CFLAGS="-Wall -Werror=strict-prototypes -Werror=missing-prototypes \
		-Werror=implicit-function-declaration \
		-Werror=pointer-arith -Werror=init-self -Werror=format=2 \
		-Werror=format-security \
		-Werror=missing-include-dirs -Werror=aggregate-return \
		-Werror=declaration-after-statement"
fi
changequote([,])dnl
AC_SUBST(WARN_CFLAGS)

# Initialize libtool
LT_PREREQ([2.2.4])
LT_INIT([disable-static])

PKG_CHECK_MODULES(BUILDDEP_MMS, [gio-unix-2.0 libsoup-2.4 >= 2.42.2 libgsystem])

AC_ARG_WITH([systemdsystemunitdir],
            AS_HELP_STRING([--with-systemdsystemunitdir=DIR], [Directory for systemd service files]),
            [],
            [with_systemdsystemunitdir=$($PKG_CONFIG --variable=systemdsystemunitdir systemd)])
if test "x$with_systemdsystemunitdir" != "xno"; then
  AC_SUBST([systemdsystemunitdir], [$with_systemdsystemunitdir])
fi
if test -n "$systemdsystemunitdir"; then
  have_systemd=yes
else
  have_systemd=no
fi
AM_CONDITIONAL(HAVE_SYSTEMD, [test x$have_systemd = xyes])

AC_CONFIG_FILES([
Makefile
])
AC_OUTPUT

echo "
    $PACKAGE $VERSION

    systemd:                     $have_systemd
"
