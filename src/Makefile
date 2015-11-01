CC=gcc
CFLAGS= -Wall -g -O2 -fPIC -ldl -shared
RM= rm -f
LIBNAME=libbufread
.PHONY: all clean

all: $(LIBNAME)
clean:
		$(RM) *.o *.so

$(LIBNAME):
		$(CC) $(CFLAGS) $(LIBNAME).c -o $(LIBNAME).so
