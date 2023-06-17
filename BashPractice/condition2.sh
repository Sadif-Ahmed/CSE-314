#!/bin/sh

read pass

if [ $pass = "abc" ] 
then
echo "Correct Password"
else
echo "Incorrect Password"
fi

