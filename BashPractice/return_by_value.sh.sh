#!/bin/bash

take_sum()
{
total=0
for i in $*
do
total=$(($total+$i))
done
}

take_sum $*`

echo $total 
