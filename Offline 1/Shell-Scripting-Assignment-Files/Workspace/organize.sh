#!/bin/bash
mkdir -p $2
mkdir -p $2/C
mkdir -p $2/Python
mkdir -p $2/Java
mkdir -p unzipped
count_test()
{
	count=0
	for files in "$1"/*
	do
	count=$(($count+1))
	done
	echo "Found $count test files"
}
if [ $# -gt 4 ]
then
if [ $5 = "-v" ]
then
count_test $3
fi
fi
for file in "$1"/*
do
#echo "${file: -11}"
mkdir -p unzipped/${file: -11}
unzip "$file" -dq unzipped/${file: -11}
done

visit_and_organise()
{
	if [ -d "$1" ]
	then
	
		for i in "$1"/*
		do
			visit_and_organise "$i" $2 -v
		done
	
	elif [ -f "$1" ]
	then
	#echo "$i"
	#echo "${i:9:7}"
	if [ "${i: -2}" =  ".c" ]
	then
	#echo "c file"
	mkdir -p $2/C/${i:9:7}
	cp -p "$i" "$2/C/${i:9:7}/main.c"	
	elif [ "${i: -3}" =  ".py" ]
	then
	#echo "python file"
	mkdir -p $2/Python/${i:9:7}
	cp -p "$i" "$2/Python/${i:9:7}/main.py"
	elif [ "${i: -5}" =  ".java" ]
	then
	#echo "java file"
	mkdir -p $2/Java/${i:9:7}
	cp -p "$i" "$2/Java/${i:9:7}/Main.java"
	fi	
	fi
}

visit_and_organise unzipped $2 -v
rm -r unzipped
