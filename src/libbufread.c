/* ========================================

   * File Name : libbufread.cpp

   * Creation Date : 31-10-2015

   * Created By : Karel Ha <mathemage@gmail.com>, <kha@cern.ch>

   ==========================================*/

#define _GNU_SOURCE
#include <dlfcn.h>
#include <stdio.h>
 
typedef int (*orig_open_f_type)(const char *pathname, int flags);

int open(const char *pathname, int flags, ...) {
  static int counter = 0;
  counter++;

  printf("Fake open #%d\n", counter);

  orig_open_f_type orig_open;
  orig_open = (orig_open_f_type)dlsym(RTLD_NEXT,"open");
  return orig_open(pathname,flags);
}
