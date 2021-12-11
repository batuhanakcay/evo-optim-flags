#!/bin/sh

COMPILE_FILES="basicmath/basicmath_small.c   basicmath/rad2deg.c  basicmath/cubic.c   basicmath/isqrt.c"
RUNFILE="basicmath_small"
ARGS=""

#COMPILE_FILES="basicmath/basicmath_large.c   basicmath/rad2deg.c  basicmath/cubic.c   basicmath/isqrt.c"
#RUNFILE="basicmath_large"
#ARGS=""

#COMPILE_FILES="bitcount/bitcnt_1.c bitcount/bitcnt_2.c bitcount/bitcnt_3.c bitcount/bitcnt_4.c bitcount/bitcnts.c bitcount/bitfiles.c bitcount/bitstrng.c bitcount/bstr_i.c"
#RUNFILE="bitcount_small"
#ARGS="75000"

#COMPILE_FILES="bitcount/bitcnt_1.c bitcount/bitcnt_2.c bitcount/bitcnt_3.c bitcount/bitcnt_4.c bitcount/bitcnts.c bitcount/bitfiles.c bitcount/bitstrng.c bitcount/bstr_i.c"
#RUNFILE="bitcount_large"
#ARGS="1125000"

#COMPILE_FILES="qsort/qsort_small.c"
#RUNFILE="qsort_small"
#ARGS="qsort/input_small.dat" 

#COMPILE_FILES="qsort/qsort_large.c"
#RUNFILE="qsort_large"
#ARGS="qsort/input_large.dat" 

#COMPILE_FILES="susan/susan.c"
#RUNFILE="susan_small"
#ARGS="susan/input_small.pgm output_small.smoothing.pgm -s" 

#COMPILE_FILES="susan/susan.c"
#RUNFILE="susan_large"
#ARGS="susan/input_large.pgm output_large.smoothing.pgm -s" 

END=30

gcc $COMPILE_FILES -o $RUNFILE -lm
for i in $(seq 1 $END); do
    bash -c "(TIMEFORMAT='%3R'; time ./$RUNFILE $ARGS > output.txt) &>> $RUNFILE.txt";
done
