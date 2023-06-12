#!/bin/bash
read pass

until [ $pass = abc ]
do 
echo Wrong Pass
read pass
done
echo Password Matched
