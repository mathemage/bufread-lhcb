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

Implement buffered read for LHCb DAQ system. Current system uses blocksizes, which are suboptimal in terms of throughput. It has been discovered that the ideal blocksize for disk-read is 16 MB. However, system call for `read` uses different sizes.

Therefore, it's more advisable to load from a disk into a temporary buffer in memory and then `read()` from this buffer instead. The size of the buffer should be 16 MB in order to exploit optimal disk-read speed.

In case of a `read()` that overlaps the end of buffer, a secondary buffer of 16 MB is available. Once it's reached, the new data is loaded into the original (primary) buffer and the roles of the primary and the secondary buffers are swapped.

.. image:: img/instructions.jpg

Comments on the source code
---------------------------

Code has been written in C language. It compiles to `.so` file. With `LD_PRELOAD` trick it can intercept system calls `open`, `close` and `read`, which are used by `cp` command.

./src/libbufread.c
~~~~~~~~~~~~~~~~~~

TODO

./include/libbufread.h
~~~~~~~~~~~~~~~~~~~~~~

TODO