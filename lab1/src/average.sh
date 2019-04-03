#!/bin/bash
sum=0
count=$#
for param in "$@"
do
let "sum = sum + param"
done
echo "avg = $(($sum / $count)) count = $count"  