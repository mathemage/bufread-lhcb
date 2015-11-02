/* ========================================

   * File Name : libbufread.cpp

   * Creation Date : 31-10-2015

   * Created By : Karel Ha <mathemage@gmail.com>, <kha@cern.ch>

   ==========================================*/

#define _GNU_SOURCE
#include <dlfcn.h>
#include <stdio.h>

#define VERBOSE

typedef int (*orig_open_f_type)(const char *pathname, int flags);
typedef int (*orig_close_f_type)(int fd);
typedef ssize_t (*orig_read_f_type)(int fd, void *buf, size_t count);

int open(const char *pathname, int flags, ...) {
  orig_open_f_type orig_open = (orig_open_f_type)dlsym(RTLD_NEXT,"open");
  int fd = orig_open(pathname, flags);
#ifdef VERBOSE
  printf("File '%s' opened with file descriptor %d...\n", pathname, fd);
#endif
  return fd;
}

int close(int fd, ...) {
  orig_close_f_type orig_close = (orig_close_f_type)dlsym(RTLD_NEXT,"close");
  int return_result = orig_close(fd);
#ifdef VERBOSE
  printf("File descriptor %d closed with the result %d...\n", fd, return_result);
#endif
  return return_result;
}

ssize_t read(int fd, void *buf, size_t count) {
  orig_read_f_type orig_read = (orig_read_f_type)dlsym(RTLD_NEXT,"read");
  ssize_t bytes_read = orig_read(fd, buf, count);
#ifdef VERBOSE
  printf("%d B read from file descriptor %d...\n", bytes_read, fd);
#endif
  return bytes_read;
}
