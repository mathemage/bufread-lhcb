#! /bin/bash

make clean
make

rm blob.out -f
LD_PRELOAD="$LD_PRELOAD:./libbufread.so" cp blob.in blob.out
diff blob.in blob.out
