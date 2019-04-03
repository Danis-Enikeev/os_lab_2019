#!/bin/bash
num=0
for i in {1..150}
do
num=$(od -An -N4 -i < /dev/urandom)
nums="$num $nums"
done
echo $nums >> randomnums.txtnoh