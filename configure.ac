C_PREREQ([2.64])
AC_INIT(AcidCam, 2.0.24, jaredbruni@gmail.com)

AC_CONFIG_HEADERS(config.h)

AM_INIT_AUTOMAKE
# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_LANG_CPLUSPLUS

# Checks for header files.
AC_CHECK_HEADERS([unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_C_INLINE

# Checks for library functions.
AC_CHECK_FUNCS([sqrt])

dnl KDE_FIND_PATH(programm-name, variable-name, list of directories,
dnl     if-not-found, test-parameter)
AC_DEFUN(KDE_FIND_PATH,
[
AC_MSG_CHECKING([for $1])
if test -n "$$2"; then
kde_cv_path="$$2";
else
kde_cache=`echo $1 | sed 'y%./+-%__p_%'`

AC_CACHE_VAL(kde_cv_path_$kde_cache,
[
kde_cv_path="NONE"
dirs="$3"
kde_save_IFS=$IFS
IFS=':'
for dir in $PATH; do
dirs="$dirs $dir"
done
IFS=$kde_save_IFS

for dir in $dirs; do
if test -x "$dir/$1"; then
if test -n "$5"
then
evalstr="$dir/$1 $5 2>&1 "
if eval $evalstr; then
kde_cv_path="$dir/$1"
break
fi
else
kde_cv_path="$dir/$1"
break
fi
fi
done

eval "kde_cv_path_$kde_cache=$kde_cv_path"

])

eval "kde_cv_path=\"`echo '$kde_cv_path_'$kde_cache`\""

fi

if test -z "$kde_cv_path" || test "$kde_cv_path" = NONE; then
AC_MSG_RESULT(not found)
$4
else
AC_MSG_RESULT($kde_cv_path)
$2=$kde_cv_path

fi
])


KDE_FIND_PATH(pkg-config, PKG_CONFIG, [${prefix}/bin ${exec_prefix}/bin ], [
AC_MSG_ERROR([Could not find pkg-config, check http://www.sdl.org])
])

LDPREFIX=${prefix}
if test  ${prefix} = "NONE"; then
LDPREFIX=/usr/local
fi

export PKG_CONFIG_PATH="${LDPREFIX}/lib/pkgconfig:$PKG_CONFIG_PATH"

OPENCV_CFLAGS=`$PKG_CONFIG opencv --cflags`
OPENCV_LDFLAGS=`$PKG_CONFIG opencv --libs`

echo "CFLAGS $OPENCV_CFLAGS"
echo "LDFLAGS $OPENCV_LDFLAGS"

AC_SUBST(OPENCV_CFLAGS)
AC_SUBST(OPENCV_LDFLAGS)


AC_OUTPUT(Makefile code/Makefile)
