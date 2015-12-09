/* ========================================

   * File Name : libbufread.cpp

   * Creation Date : 31-10-2015

   * Created By : Karel Ha <mathemage@gmail.com>, <kha@cern.ch>

   ==========================================*/

#include "libbufread.h"

#define VERBOSE
#define BLOCKSIZE 16777216     // 16 MB

const int open_files_limit = 1024;
void **primary_buffers = NULL;
void **secondary_buffers = NULL;
int *current_positions = NULL;
ssize_t *bytes_available = NULL;

void init_buffers() {
  if (primary_buffers == NULL) {
    primary_buffers = calloc(open_files_limit, sizeof(void*));
    secondary_buffers = calloc(open_files_limit, sizeof(void*));
    current_positions = (int *) calloc(open_files_limit, sizeof(int));
    bytes_available = (ssize_t *) calloc(open_files_limit, sizeof(int));

    int i;
    for (i = 0; i < open_files_limit; i++) {
      primary_buffers[i] = NULL;
      secondary_buffers[i] = NULL;
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
  primary_buffers[fd] = malloc(BLOCKSIZE); 
  secondary_buffers[fd] = malloc(BLOCKSIZE); 

  // initial read
  orig_read_f_type orig_read = (orig_read_f_type)dlsym(RTLD_NEXT,"read");
  bytes_available[fd] = orig_read(fd, primary_buffers[fd], BLOCKSIZE);
  bytes_available[fd] += orig_read(fd, secondary_buffers[fd], BLOCKSIZE);

  return fd;
}

int close(int fd, ...) {
  orig_close_f_type orig_close = (orig_close_f_type)dlsym(RTLD_NEXT,"close");
  int return_result = orig_close(fd);
#ifdef VERBOSE
  printf("File descriptor %d closed with the result %d...\n", fd, return_result);
#endif
  if (primary_buffers[fd] != NULL) {
    free(primary_buffers[fd]);
    primary_buffers[fd] = NULL;
  }
  if (secondary_buffers[fd] != NULL) {
    free(secondary_buffers[fd]);
    secondary_buffers[fd] = NULL;
  }
  return return_result;
}

ssize_t read(int fd, void *buf, size_t count) {
  orig_read_f_type orig_read = (orig_read_f_type)dlsym(RTLD_NEXT,"read");
  ssize_t bytes_read = 0;
  if (primary_buffers[fd] == NULL && secondary_buffers[fd] != NULL) {
    /* 
    ssize_t bytes_to_load = 

    if (eof_offsets[fd] == EOF_NOT_REACHED) {
      // TODO what if reached end of buffer
    } else {
      if (current_positions[fd] <= eof_offsets[fd]) {
        int remaining_bytes = eof_offsets[fd] - current_positions[fd] + 1;
        if (count > remaining_bytes) {
          memcpy(buf, buffers[fd] + current_positions[fd], remaining_bytes);
          eof_offsets[fd] = EOF_REACHED;
          bytes_read = remaining_bytes;
        } else {
          memcpy(buf, buffers[fd] + current_positions[fd], count);
          current_positions[fd] += count;
          bytes_read = count;
        }
      } else {
        // TODO circular read
      }
    } */

#ifdef VERBOSE
    printf("%zd B read from file descriptor %d using buffer...\n", bytes_read, fd);
#endif
  } else {       // if some problem occured, revert to the regular system read
    bytes_read = orig_read(fd, buf, count);
#ifdef VERBOSE
    printf("%zd B read from file descriptor %d...\n", bytes_read, fd);
#endif
  }
  return bytes_read;
}
