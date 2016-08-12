PROG=	robin
SRCS=	wrlib/wrmpool.c wrlib/wrhash.c wrlib/wrstring.c wrlib/misc.c wrlib/wrio.c \
	wrlib/http.c wrlib/request.c wrlib/analysis.c wrlib/mg_compat.c \
	main.c cgi.c  
COPT=	-W -Os -s -D_POSIX_SOURCE -D_BSD_SOURCE
WINOPT=	/O2 /Oi /Os /GT /GL /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /EHsc /MD /Gy \
/W3 /nologo /Zi /D "_MBCS" /D_CRT_SECURE_NO_WARNINGS /D_CRT_SECURE_NO_DEPRECATE

windows:
	cl $(WINOPT) $(SRCS) /link /incremental:no \
		/out:$(PROG).exe ws2_32.lib advapi32.lib

	del *.obj *.bak *.pdb

linux:
	gcc $(COPT) $(SRCS) -ldl -lpthread -o $(PROG)
