/* Minimal config.h for building readline on macOS without autoconf. */
#define HAVE_CONFIG_H 1
#define HAVE_STDLIB_H 1
#define HAVE_UNISTD_H 1
#define HAVE_STRING_H 1
#define HAVE_LOCALE_H 1
#define HAVE_SYS_FILE_H 1
#define HAVE_SYS_TIME_H 1
#define HAVE_SYS_IOCTL_H 1
#define HAVE_ISASCII 1
#define HAVE_WCTYPE_H 1
#define HAVE_WCHAR_H 1
#define HAVE_DIRENT_H 1
#define HAVE_LSTAT 1
#define HAVE_MEMMOVE 1
#define HAVE_PUTENV 1
#define HAVE_SETENV 1
#define HAVE_TIMEVAL 1
#define HAVE_GETTIMEOFDAY 1
#define HAVE_SYSCONF 1
#define HAVE_POSIX_SIGNALS 1
#define HAVE_STRCOLL 1
#define HAVE_ISWCTYPE 1
#define HAVE_MBSRTOWCS 1
#define HAVE_WCWIDTH 1
#define VOID_SIGHANDLER 1
#ifndef READLINE_LIBRARY
# define READLINE_LIBRARY 1
#endif
