#!/bin/sh
echo "accfence2 "
aprun -n 4 ./accfence2
aprun -n 4 -N 1 ./accfence2
echo "accfence2_am "
aprun -n 4 ./accfence2_am
aprun -n 4 -N 1 ./accfence2_am
echo "contention_put "
aprun -n 4 ./contention_put
aprun -n 4 -N 1 ./contention_put
echo "contention_putget "
aprun -n 4 ./contention_putget
aprun -n 4 -N 1 ./contention_putget
echo "fetchandadd "
aprun -n 7 ./fetchandadd
aprun -n 7 -N 2 ./fetchandadd
echo "fetchandadd_am "
aprun -n 7 ./fetchandadd_am
aprun -n 7 -N 2 ./fetchandadd_am
echo "fetchandadd_tree "
aprun -n 7 ./fetchandadd_tree
aprun -n 7 -N 2 ./fetchandadd_tree
echo "fetchandadd_tree_am "
aprun -n 7 ./fetchandadd_tree_am
aprun -n 7 -N 2 ./fetchandadd_tree_am
echo "getgroup "
aprun -n 4 ./getgroup
aprun -n 4 -N 1 ./getgroup
#ircpi is interactive
#echo "ircpi "
#aprun -n 5 ./ircpi
echo "lockcontention "
aprun -n 3 ./lockcontention
aprun -n 3 -N 1 ./lockcontention
echo "lockcontention2 with 4 processes "
aprun -n 4 ./lockcontention2
aprun -n 4 -N 1 ./lockcontention2
echo "lockcontention2 with 8 processes "
aprun -n 8 ./lockcontention2
aprun -n 8 -N 2 ./lockcontention2
echo "lockcontention3 with 8 processes "
aprun -n 8 ./lockcontention3
aprun -n 8 -N 2 ./lockcontention3
echo "locknull "
aprun -n 2 ./locknull
aprun -n 2 -N 1 ./locknull
echo "lockopts "
aprun -n 2 ./lockopts
aprun -n 2 -N 1 ./lockopts
echo "mixedsync "
aprun -n 4 ./mixedsync
aprun -n 4 -N 1 ./mixedsync
echo "nullpscw "
aprun -n 7 ./nullpscw
aprun -n 7 -N 2 ./nullpscw
echo "put_base "
aprun -n 2 ./put_base
aprun -n 2 -N 1 ./put_base
echo "put_bottom "
aprun -n 2 ./put_bottom
aprun -n 2 -N 1 ./put_bottom
echo "selfrma "
aprun -n 1 ./selfrma
aprun -n 1 -N 1 ./selfrma
echo "strided_acc_indexed "
aprun -n 2 ./strided_acc_indexed
aprun -n 2 -N 1 ./strided_acc_indexed
echo "strided_acc_onelock "
aprun -n 2 ./strided_acc_onelock
aprun -n 2 -N 1 ./strided_acc_onelock
echo "strided_acc_subarray "
aprun -n 2 ./strided_acc_subarray
aprun -n 2 -N 1 ./strided_acc_subarray
echo "strided_get_indexed "
aprun -n 2 ./strided_get_indexed
aprun -n 2 -N 1 ./strided_get_indexed
echo "strided_putget_indexed "
aprun -n 2 ./strided_putget_indexed
aprun -n 2 -N 1 ./strided_putget_indexed
echo "test1 "
aprun -n 2 ./test1
aprun -n 2 -N 1 ./test1
echo "test1_am "
aprun -n 2 ./test1_am
aprun -n 2 -N 1 ./test1_am
echo "test1_dt "
aprun -n 2 ./test1_dt
aprun -n 2 -N 1 ./test1_dt
echo "test2 "
aprun -n 2 ./test2
aprun -n 2 -N 1 ./test2
echo "test2_am "
aprun -n 2 ./test2_am
aprun -n 2 -N 1 ./test2_am
echo "test3 "
aprun -n 2 ./test3
aprun -n 2 -N 1 ./test3
echo "test3_am "
aprun -n 2 ./test3_am
aprun -n 2 -N 1 ./test3_am
echo "test4 "
aprun -n 2 ./test4
aprun -n 2 -N 1 ./test4
echo "test4_am "
aprun -n 2 ./test4_am
aprun -n 2 -N 1 ./test4_am
echo "test5 "
aprun -n 2 ./test5
aprun -n 2 -N 1 ./test5
echo "test5_am "
aprun -n 2 ./test5_am
aprun -n 2 -N 1 ./test5_am
echo "transpose1 "
aprun -n 2 ./transpose1
aprun -n 2 -N 1 ./transpose1
echo "transpose2 "
aprun -n 2 ./transpose2
aprun -n 2 -N 1 ./transpose2
echo "transpose3 "
aprun -n 2 ./transpose3
aprun -n 2 -N 1 ./transpose3
echo "transpose4 "
aprun -n 2 ./transpose4
aprun -n 2 -N 1 ./transpose4
echo "transpose5 "
aprun -n 2 ./transpose5
aprun -n 2 -N 1 ./transpose5
echo "transpose6 "
aprun -n 1 ./transpose6
echo "transpose7 "
aprun -n 2 ./transpose7
aprun -n 2 -N 1 ./transpose7
echo "window_creation "
aprun -n 2 ./window_creation
aprun -n 2 -N 1 ./window_creation
echo "wintest "
aprun -n 2 ./wintest
aprun -n 2 -N 1 ./wintest
