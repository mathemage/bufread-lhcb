 
/* ========================================

   * File Name :

   * Creation Date : 07-12-2015

   * Last Modified : Wed 16 Mar 2016 01:02:25 AM CET

   * Created By : Karel Ha <mathemage@gmail.com>

   ==========================================*/

#ifndef __LIBBUFREAD_H__
#define __LIBBUFREAD_H__

#define _GNU_SOURCE
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <unistd.h>

#define MIN(a,b) ({ \
  __typeof__ (a) _a = (a); \
  __typeof__ (b) _b = (b); \
  _a < _b ? _a : _b; \
})

typedef int (*orig_open_f_type)(const char *pathname, int flags);
typedef int (*orig_close_f_type)(int fd);
typedef ssize_t (*orig_read_f_type)(int fd, void *buf, size_t count);
typedef char* (*orig_get_current_dir_name_f_type)(void);

void init_buffers();
void trim_trailing_newline(char * str);
char * append_slash(char * str);
int is_prefix(char *prefix, const char *str);
char * prepend_cur_dir(char * str);
int is_in_whitelist(const char *pathname);
int open(const char *pathname, int flags, ...);
int close(int fd, ...);
ssize_t read(int fd, void *buf, size_t count);

#endif
