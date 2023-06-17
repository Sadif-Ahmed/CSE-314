#!/bin/sh

read n

if [ $n -gt 100 ] 
then
echo "Greater Than 100"
elif [ $n -gt 50 ]
then
echo "Greater Than 50"
else
echo "Smaller than 50"
fi

