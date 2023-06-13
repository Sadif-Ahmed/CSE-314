#!/bin/bash
diff -q a.txt b.txt > res.txt
temp=0
wc -L res.txt >> $temp
echo $temp
if [ $temp -eq 0 ]
then
echo "Matches."
else
echo "Does not Match."
fi
