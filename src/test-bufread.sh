#! /bin/bash

make clean
make

dir="/tmp/io-testfiles"

for infile in $dir/*.in
do
  outfile=$dir/$(basename "$infile" in)out
  >$outfile
  chmod 777 $outfile
  cmd="LD_PRELOAD="$LD_PRELOAD:./libbufread.so" cp $infile $outfile -f && diff $infile $outfile"
  echo "Executing: $cmd"
  bash -c "$cmd" || { echo "Failure: $cmd"; exit 1; }
done

echo
echo "Passed all tests!"
