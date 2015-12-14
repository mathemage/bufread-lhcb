 
/* ========================================

   * File Name :

   * Creation Date : 07-12-2015

   * Last Modified : Mon 14 Dec 2015 03:56:08 PM CET

   * Created By : Karel Ha <mathemage@gmail.com>

   ==========================================*/

#ifndef __LIBBUFREAD_H__
#define __LIBBUFREAD_H__

#define _GNU_SOURCE
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MIN(a,b) ({ \
  __typeof__ (a) _a = (a); \
  __typeof__ (b) _b = (b); \
  _a < _b ? _a : _b; \
})

typedef int (*orig_open_f_type)(const char *pathname, int flags);
typedef int (*orig_close_f_type)(int fd);
typedef ssize_t (*orig_read_f_type)(int fd, void *buf, size_t count);

void init_buffers();
int open(const char *pathname, int flags, ...);
int close(int fd, ...);
ssize_t read(int fd, void *buf, size_t count);

#endif
