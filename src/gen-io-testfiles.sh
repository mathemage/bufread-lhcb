#! /bin/bash
# according to http://unix.stackexchange.com/questions/32988/why-does-dd-from-dev-random-give-different-file-sizes

dir="/tmp/io-testfiles"
mkdir $dir -p

bs=16777216
factors="0 0.5 1 1.5 2 2.5 3 3.5 4 4.5 100 1000"

for f in $factors
do
  #bytesize=$(echo "floor($f * $bs)" | bc)
  bytesize=$(python -c "from math import floor; print int(floor($f*$bs))")
  randfile="$dir/$bytesize-B.in"
  cmd="head -c $bytesize < /dev/urandom > $randfile"
  bash -c "$cmd"
  echo $cmd
done
