 
/* ========================================

   * File Name :

   * Creation Date : 07-12-2015

   * Last Modified : Wed 09 Dec 2015 02:30:42 PM CET

   * Created By : Karel Ha <mathemage@gmail.com>

   ==========================================*/

#ifndef __LIBBUFREAD_H__
#define __LIBBUFREAD_H__

#define _GNU_SOURCE
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void safe_dealloc(void **ptr);
void safe_calloc(void **ptr, size_t nmemb, size_t size);

typedef int (*orig_open_f_type)(const char *pathname, int flags);
typedef int (*orig_close_f_type)(int fd);
typedef ssize_t (*orig_read_f_type)(int fd, void *buf, size_t count);

#endif
