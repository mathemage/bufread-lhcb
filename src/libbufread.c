/* ========================================

   * File Name : libbufread.cpp

   * Creation Date : 31-10-2015

   * Created By : Karel Ha <mathemage@gmail.com>, <kha@cern.ch>

   ==========================================*/

#include "libbufread.h"

#define VERBOSE
#define BLOCKSIZE 16777216     // 16 MB

const int open_files_limit = 1024;
void **buffers = NULL;
const int bufsize = 2 * BLOCKSIZE;
int *buf_offsets;
// TODO eof_indices

void init_buffers() {
  if (buffers == NULL) {
    buffers = calloc(open_files_limit, sizeof(void*));
    buf_offsets = (int *) calloc(open_files_limit, sizeof(int));
    int i;
    for (i = 0; i < open_files_limit; i++) {
      buffers[i] = NULL;
    }
  }
}

int open(const char *pathname, int flags, ...) {
  init_buffers();
  orig_open_f_type orig_open = (orig_open_f_type)dlsym(RTLD_NEXT,"open");
  int fd = orig_open(pathname, flags);
#ifdef VERBOSE
  printf("File '%s' opened with file descriptor %d...\n", pathname, fd);
#endif
  buffers[fd] = malloc(bufsize); 
  buf_offsets[fd] = 0;

  // initial read
  orig_read_f_type orig_read = (orig_read_f_type)dlsym(RTLD_NEXT,"read");
  ssize_t bytes_read = orig_read(fd, buffers[fd], bufsize);
  if (bytes_read < bufsize) {
    // TODO EOF not reached
  } else {
    // TODO EOF reached
  }

  return fd;
}

int close(int fd, ...) {
  orig_close_f_type orig_close = (orig_close_f_type)dlsym(RTLD_NEXT,"close");
  int return_result = orig_close(fd);
#ifdef VERBOSE
  printf("File descriptor %d closed with the result %d...\n", fd, return_result);
#endif
  if (buffers[fd] != NULL) {
    free(buffers[fd]);
    buffers[fd] = NULL;
  }
  return return_result;
}

ssize_t read(int fd, void *buf, size_t count) {
  orig_read_f_type orig_read = (orig_read_f_type)dlsym(RTLD_NEXT,"read");
  ssize_t bytes_read;
  if (buffers[fd] != NULL) {
    // TODO what if reached end of buffer
    memcpy(buf, buffers[fd] + buf_offsets[fd], count);
    buf_offsets[fd] += count;

    // TODO do differently for corner cases
    bytes_read = count;
#ifdef VERBOSE
    printf("%zd B read from file descriptor %d using buffer...\n", bytes_read, fd);
#endif
  } else {
    bytes_read = orig_read(fd, buf, count);
#ifdef VERBOSE
    printf("%zd B read from file descriptor %d...\n", bytes_read, fd);
#endif
  }
  return bytes_read;
}
