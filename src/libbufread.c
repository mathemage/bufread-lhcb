/* ========================================

   * File Name : libbufread.cpp

   * Creation Date : 31-10-2015

   * Created By : Karel Ha <mathemage@gmail.com>, <kha@cern.ch>

   ==========================================*/

#include "libbufread.h"

// #define VERBOSE
#define BLOCKSIZE 16777216     // 16 MB
typedef int bool;
const bool true = 1;
const bool false = 0;

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

void trim_trailing_newline(char * str) {
  size_t lenstr = strlen(str);
  if (lenstr > 0 && str[lenstr - 1] == '\n') {
    str = (char *) realloc(str, lenstr);              // lenstr <= strlen() doesn't account for final '\0'
    str[lenstr - 1] = '\0';
  }
}

char * append_slash(char * str) {
  size_t lenstr = strlen(str);
  if (lenstr > 0 && str[lenstr - 1] != '/') {
    str = (char *) realloc(str, lenstr + 2);          // +2 <= '/' and '\0'
    return strcat(str, "/");
  }
  return str;
}

int is_prefix(char *prefix, const char *str) {
  size_t lenprefix = strlen(prefix);
  size_t lenstr = strlen(str);
#ifdef VERBOSE
  printf("is_prefix(\"%s\", \"%s\") == %d\n\n",
      prefix, str, lenstr < lenprefix ? false : strncmp(prefix, str, lenprefix) == 0);
#endif
  return lenstr < lenprefix ? false : strncmp(prefix, str, lenprefix) == 0;
}

bool is_in_whitelist(const char *pathname) {
  FILE * fp = fopen("whitelist.conf", "r");
  if (fp == NULL) {
    fprintf(stderr, "File whitelist.conf couldn't be opened!\n");
    return false;
  }

  char * dir = NULL;
  size_t len = 0;
  ssize_t read;

  while ((read = getline(&dir, &len, fp)) != -1) {
    if (read > 0) {                 // skip empty lines
      // sanitize dir path
      trim_trailing_newline(dir);
      dir = append_slash(dir);

#ifdef VERBOSE
      printf("dir == \"%s\"\n", dir);
#endif
      if (is_prefix(dir, pathname) == true) {
        return true;
      }
    }
  }

  fclose(fp);
  if (dir) {
    free(dir);
  }

  return false;
}

int open(const char *pathname, int flags, ...) {
  init_buffers();
  orig_open_f_type orig_open = (orig_open_f_type)dlsym(RTLD_NEXT,"open");
  int fd = orig_open(pathname, flags);
#ifdef VERBOSE
  printf("File '%s' opened with file descriptor %d...\n", pathname, fd);
#endif
  if (is_in_whitelist(pathname) == true) {
    primary_buffers[fd] = malloc(BLOCKSIZE); 
    secondary_buffers[fd] = malloc(BLOCKSIZE); 
    current_positions[fd] = 0;

    // initial read
    orig_read_f_type orig_read = (orig_read_f_type)dlsym(RTLD_NEXT,"read");
    bytes_available[fd] = orig_read(fd, primary_buffers[fd], BLOCKSIZE);
    bytes_available[fd] += orig_read(fd, secondary_buffers[fd], BLOCKSIZE);
  }

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
  if (primary_buffers[fd] != NULL && secondary_buffers[fd] != NULL) {
    while (count > 0 && bytes_available[fd] > 0) {
      ssize_t bytes_to_load = MIN(count,
                                MIN(BLOCKSIZE - current_positions[fd], bytes_available[fd]));
      memcpy(buf, primary_buffers[fd] + current_positions[fd], bytes_to_load);
      buf += bytes_to_load;
      current_positions[fd] += bytes_to_load;
      bytes_available[fd] -= bytes_to_load;
      count -= bytes_to_load;
      bytes_read += bytes_to_load;

      if (current_positions[fd] >= BLOCKSIZE) {     // secondary buffer reached
        // swap buffers
        void *tmp = primary_buffers[fd];
        primary_buffers[fd] = secondary_buffers[fd];
        secondary_buffers[fd] = tmp;

        bytes_available[fd] += orig_read(fd, secondary_buffers[fd], BLOCKSIZE);
        current_positions[fd] = 0;
      }
    }
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
