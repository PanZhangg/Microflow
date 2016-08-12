#ifndef __MG_COMPAT_H__
#define __MG_COMPAT_H__

#ifdef __cplusplus
extern "C"{
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdarg.h>

#ifdef _WIN32
#include <io.h>
#include <direct.h>
#include <winsock2.h>
#include <process.h>

/*
#if !defined(_POSIX_)
#define _POSIX_	
#endif
*/

typedef HANDLE pthread_mutex_t;
typedef HANDLE pthread_cond_t;

#define  WR_SOCK_ERRNO WSAGetLastError()
#define	 WR_EINTR WSAEINTR
#define  EWOULDBLOCK WSAEWOULDBLOCK

#if !defined(S_ISDIR)
#define S_ISDIR(x)		((x) & _S_IFDIR)
#endif /* S_ISDIR */

/*would conflict with iostream*/
#ifndef __cplusplus
#define	write(x, y, z)		_write(x, y, (unsigned) z)
#define	read(x, y, z)		_read(x, y, (unsigned) z)
#define	open(x, y)			_open(x, y)
#define	lseek(x, y, z)		_lseek(x, y, z)
#define	close(x)			_close(x)
#endif

#define chdir(x)			_chdir(x)

#else				/* UNIX  specific	*/
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/mman.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>

#include <pwd.h>
#include <iconv.h>
#include <unistd.h>
#include <dirent.h>
#include <dlfcn.h>
#include <pthread.h>
#define	DIRSEP			'/'
#define	IS_DIRSEP_CHAR(c)	((c) == '/')
#define	O_BINARY		0
#define	closesocket(a)		close(a)
#define	ERRNO			errno
#define	INVALID_SOCKET		(-1)
typedef int SOCKET;

#define  WR_SOCK_ERRNO errno
#define	 WR_EINTR EINTR
#endif /* End of Windows and UNIX specific includes */


#if !defined(MIN)
#define MIN(a ,b) ((a)<(b)?(a):(b))
#endif

#if !defined(MAX)
#define MAX(a ,b) ((a)>(b)?(a):(b))
#endif

/*defined in mg_compat.c*/
int start_thread(void * (*func)(void *), void *param);

#ifdef __cplusplus
}
#endif

#endif

