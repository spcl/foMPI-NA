#!/bin/sh
echo "compare_and_swap "
aprun -n 4 ./compare_and_swap
aprun -n 4 -N 1 ./compare_and_swap
echo "flush "
aprun -n 4 ./flush
aprun -n 4 -N 1 ./flush
echo "get_acc_local "
aprun -n 2 ./get_acc_local
aprun -n 2 -N 1 ./get_acc_local
echo "get_accumulate_short "
aprun -n 4 ./get_accumulate_short
aprun -n 4 -N 1 ./get_accumulate_short
echo "get_accumulate_long "
aprun -n 4 ./get_accumulate_long
aprun -n 4 -N 1 ./get_accumulate_long
echo "get_accumulate_double "
aprun -n 4 ./get_accumulate_double
aprun -n 4 -N 1 ./get_accumulate_double
echo "get_accumulate_int "
aprun -n 4 ./get_accumulate_int
aprun -n 4 -N 1 ./get_accumulate_int
echo "get_accumulate_short_derived "
aprun -n 4 ./get_accumulate_short_derived
aprun -n 4 -N 1 ./get_accumulate_short_derived
echo "get_accumulate_long_derived "
aprun -n 4 ./get_accumulate_long_derived
aprun -n 4 -N 1 ./get_accumulate_long_derived
echo "get_accumulate_double_derived "
aprun -n 4 ./get_accumulate_double_derived
aprun -n 4 -N 1 ./get_accumulate_double_derived
echo "get_accumulate_int_derived "
aprun -n 4 ./get_accumulate_int_derived
aprun -n 4 -n 1 ./get_accumulate_int_derived
echo "linked_list "
aprun -n 4 ./linked_list
aprun -n 4 -N 1 ./linked_list
echo "linked_list_fop "
aprun -n 4 ./linked_list_fop
aprun -n 4 -N 1 ./linked_list_fop
echo "linked_list_lockall "
aprun -n 4 ./linked_list_lockall
aprun -n 4 -N 1 ./linked_list_lockall
echo "manyrma2 "
aprun -n 2 ./manyrma2
aprun -n 2 -N 1 ./manyrma2
echo "reqops "
aprun -n 4 ./reqops
aprun -n 4 -N 1 ./reqops
echo "win_dynamic_acc "
aprun -n 4 ./win_dynamic_acc
aprun -n 4 -N 1 ./win_dynamic_acc
