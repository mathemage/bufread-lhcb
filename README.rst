============
BUFREAD-LHCB
============

:Author: Karel Ha
:Contact: karel.ha@cern.ch, mathemage@gmail.com
:Created on: $Date: Sat Oct 31 2015 $

GitHub repo
-----------

Move to a folder in AFS::

  mathemage@mathemage-TTL-TEKNOPRO:~$ cd /afs/cern.ch/user/k/kha/
  mathemage@mathemage-TTL-TEKNOPRO:/afs/cern.ch/user/k/kha$ mkdir bufread-lhcb

and clone the created repo on GitHub::

  mathemage@mathemage-TTL-TEKNOPRO:/afs/cern.ch/user/k/kha$ git clone git@github.com:mathemage/bufread-lhcb.git

Instructions
------------

Implement buffered read for LHCb DAQ system. Current system uses blocksizes, which are suboptimal in terms of throughput. It has been discovered that the ideal blocksize for disk-read is 16 MB. However, system call for `read()` uses different sizes.

Therefore, it's more advisable to load from a disk into a temporary buffer in memory and then `read()` from this buffer instead. The size of the buffer should be 16 MB in order to exploit optimal disk-read speed.

In case of a `read()` that overlaps the end of buffer, a secondary buffer of 16 MB is available. Once it's reached, the new data is loaded into the original (primary) buffer and the roles of the primary and the secondary buffers are swapped.

.. image:: img/instructions.jpg

Comments on the source code
---------------------------

Code has been written in C language. It compiles to `.so` file. With `LD_PRELOAD` trick it can intercept system calls `open()`, `close()` and `read()`, which are used by `cp` command.

./include/libbufread.h
~~~~~~~~~~~~~~~~~~~~~~

- `MIN()` = preprocessor macro for minimum of two values
- `*orig_open_f_type`, `*orig_close_f_type` resp. `*orig_read_f_type` = pointer to functions data type, for retrieving original system version of syscalls `open()`, `close()` resp. `read()`

./src/libbufread.c
~~~~~~~~~~~~~~~~~~

- `init_buffers()` = allocate and initialize memory (array of arrays) for buffers (done only once in the program)
- `open()` = intercepted version of `open()`; primary and secondary buffers (on the index of file descriptor) are allocated and pre-filled with data from file
- `close()` = intercepted version of `close()`; primary and secondary buffers (corresponding to the file descriptor) are deallocated
- `read()` = intercepted version of `read()`; see section `Algorithm`_ for the explanation

Algorithm
---------

.. image:: img/algorithm.jpg

Legend
~~~~~~

- `bytes_to_load`, `btl` = the maximum number of bytes that are possible to read at this moment, at the current step. It is set to minimum of `count`, `bs - cur_pos` (number of bytes left to the end of the primary buffer) and `ba`, because it may not be greater than any of these three values. The three diagrams of buffers shows the three possible scenarios:

  1. the one leading from `count` is the case, where `count` is the minimum of all three values. Note that remaining bytes now reside only in the primary buffer.
  2. the one leading from `bs - cur_pos` is the case, where `bs - cur_pos` is the minimum of all three values.

    - If `count` or `ba` are strictly greater, then `cur_pos` will reach the beginning of the secondary buffer. In that case, the roles of primary and secondary buffer will be swapped and the secondary one will be pre-filled with data from the file.

  3. the one leading from `ba` is the case, where `ba` is the minimum of all three values. 

    - If `count` is strictly greater, then there's not enough bytes until the end of file and via the `bytes_to_load` this will reflect in the total `bytes_read` (there have been less bytes read than requested by `read()`.
    - If `bs - cur_pos` is strictly greater, then all bytes available reside in the current (primary) buffer. That's why the secondary buffer is striked through.

- `count` = the number of requested bytes left to read. This number is decreased accordingly with every load from primary buffer to the final buffer `buf`.
- `bs` = the size of the primary and also secondary buffer. It is set to 16 MB.
- `cur_pos`, `cur_pos[]`, `cur_pos[fd]`, in the source code also called `current_positions[]` = the number of bytes from the beginning of the primary buffer that have been already used / loaded. It advances every time the data are copied from the primary buffer to the final buffer `buf`. There's one integer variable per each file descriptor `fd`.
- `ba` = "bytes available". The number of unread bytes from the file that have been loaded to the primary and secondary buffer, but they haven't been copied to the final buffer `buf` yet. `ba` doesn't have to be a multiple of `bs`, in case the end of file is reached (valid bytes from the file don't reside the whole buffer, see the diagram where `ba` is the minimum).
- `buf` = the final buffer given as an argument to `read()`
- `prim[]`, `prim[fd]` = the primary buffer corresponding to the file descriptor `fd`
- `sec[]`, `sec[fd]` = the secondary buffer corresponding to the file descriptor `fd`
- `bytes_read` = the total number of bytes copied from the primary and secondary buffers to `buf`, altogether across all steps. It usually equals to `count`, unless the end of file is reached prematurely (in which case it equals to the filesize). `bytes_read` serves as the return value of `read()`
- `swap_buffers()`, `swap_buffers(fd)` = exchange the role of the primary and secondary buffer by swapping their pointers
- `orig_read()` = the original system version of the syscall `read()`

Description of the algorithm
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

TODO

Testing
-------

Testing is done by copying chosen input files and checking, whether the output files differ from them. Using `LD_PRELOAD` trick, the regular `read()` is intercepted and the bufread version is used instead.

./src/gen-io-testfiles.sh
~~~~~~~~~~~~~~~~~~~~~~~~~

This script generates several input files with random content using `/dev/urandom`. The size of the files are multiples (see `$factors` in the script) of the chosen blocksize `$bs` (set to 16 MB). The writing permission are removed at the end, since these files serve as exclusively input files.

./src/test-bufread.sh
~~~~~~~~~~~~~~~~~~~~~

This script

1. recompiles the library.
2. For every input file in the given directory `$dir`

   a. creates an empty output file with `*.out` extension.
   b. adjust permission rights accordingly.
   c. intercept `read()` by adding the bufread library to `LD_PRELOAD` variable.
   d. copy from the input file to the output file.
   e. compares the input file and the output file by `diff` and exits with failure if they differ.

TODO
----

- discriminate based on filepaths given by a config file
- check for file permissions
