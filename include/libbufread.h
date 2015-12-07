 
/* ========================================

   * File Name :

   * Creation Date : 07-12-2015

   * Last Modified : Mon 07 Dec 2015 05:06:53 PM CET

   * Created By : Karel Ha <mathemage@gmail.com>

   ==========================================*/

#ifndef __LIBBUFREAD_H__
#define __LIBBUFREAD_H__

#define _GNU_SOURCE
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef int (*orig_open_f_type)(const char *pathname, int flags);
typedef int (*orig_close_f_type)(int fd);
typedef ssize_t (*orig_read_f_type)(int fd, void *buf, size_t count);

#endif
