#!/bin/bash

total=0
for ((i=0;i<100;i++))
do
total=$(($total+$i))
done
echo $total
