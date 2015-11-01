#! /bin/bash

make clean
make

LD_PRELOAD="$LD_PRELOAD:./libbufread.so" cp blob.in blob.out
diff blob.in blob.out
