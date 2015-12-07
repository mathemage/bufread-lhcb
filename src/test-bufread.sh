#! /bin/bash

make clean
make

>blob.out
chmod 777 blob.out
LD_PRELOAD="$LD_PRELOAD:./libbufread.so" cp blob.in blob.out
diff blob.in blob.out
