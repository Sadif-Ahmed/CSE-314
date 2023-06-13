#!/bin/bash
mkdir -p $2
mkdir -p $2/C
mkdir -p $2/Python
mkdir -p $2/Java
mkdir -p unzipped

for file in "$1"/*
do
#echo "${file: -11}"
mkdir -p unzipped/${file: -11}
unzip -oq "$file" -d unzipped/${file: -11}
done
count_test()
{
	count=0
	for files in "$1"/*
	do
	count=$(($count+1))
	done
	echo "Found $count test files"
}
visit_and_organise()
{
	if [ -d "$1" ]
	then
	
		for i in "$1"/*
		do
			visit_and_organise "$i" $2 $3 $4 $5 $6
		done
	
	elif [ -f "$1" ]
	then
	#echo "$i"
	#echo "${i:9:7}"
	if [ $3 -eq 1 ]
	then
	echo "Organising files for ${i:9:7} "
	fi
	if [ "${i: -2}" =  ".c" ]
	then
	#echo "c file"
	mkdir -p $2/C/${i:9:7}
	cp -p "$i" "$2/C/${i:9:7}/main.c"
	if [ $4 -eq 0 ]
	then
	if [ $3 -eq 1 ]
	then
	echo "Executing files for ${i:9:7} "
	fi
	gcc "$2/C/${i:9:7}/main.c" -o "$2/C/${i:9:7}/main.out"	
	idx=1;
	matched=0
	not_matched=0
	for files in "$5"/*
	do
	#echo "$files"
	./"$2/C/${i:9:7}/main.out"  < "$files" > "$2/C/${i:9:7}/out$idx.txt"
	flag=0
	flag=$(diff -y --suppress-common-lines "$2/C/${i:9:7}/out$idx.txt" "$6/ans$idx.txt" | wc -l)
	if [ $flag -eq 0 ]
	then
	matched=$(($matched+1))
	else
	not_matched=$(($not_matched+1))
	fi
	idx=$(($idx+1))
	done
	echo "${i:9:7},C,$matched,$not_matched" >> $2/result.csv
	fi
	elif [ "${i: -3}" =  ".py" ]
	then
	#echo "python file"
	mkdir -p $2/Python/${i:9:7}
	cp -p "$i" "$2/Python/${i:9:7}/main.py"
	if [ $4 -eq 0 ]
	then
	if [ $3 -eq 1 ]
	then
	echo "Executing files for ${i:9:7} "
	fi
	idx=1;
	matched=0
	not_matched=0
	for files in "$5"/*
	do
	#echo "$files"
	python3 "$2/Python/${i:9:7}/main.py"  < "$files" > "$2/Python/${i:9:7}/out$idx.txt"
	flag=0
	flag=$(diff -y --suppress-common-lines "$2/Python/${i:9:7}/out$idx.txt" "$6/ans$idx.txt" | wc -l)
	if [ $flag -eq 0 ]
	then
	matched=$(($matched+1))
	else
	not_matched=$(($not_matched+1))
	fi
	idx=$(($idx+1))
	done
	echo "${i:9:7},Python,$matched,$not_matched" >> $2/result.csv
	fi
	elif [ "${i: -5}" =  ".java" ]
	then
	#echo "java file"
	mkdir -p $2/Java/${i:9:7}
	cp -p "$i" "$2/Java/${i:9:7}/Main.java"	
	javac "$2/Java/${i:9:7}/Main.java"
	if [ $4 -eq 0 ]
	then
	if [ $3 -eq 1 ]
	then
	echo "Executing files for ${i:9:7} "
	fi
	idx=1;
	matched=0
	not_matched=0
	for files in "$5"/*
	do
	#echo "$files"
	java -cp "$2/Java/${i:9:7}" Main  < "$files" > "$2/Java/${i:9:7}/out$idx.txt"
	flag=0
	flag=$(diff -y --suppress-common-lines "$2/Java/${i:9:7}/out$idx.txt" "$6/ans$idx.txt" | wc -l)
	if [ $flag -eq 0 ]
	then
	matched=$(($matched+1))
	else
	not_matched=$(($not_matched+1))
	fi
	idx=$(($idx+1))
	done
	echo "${i:9:7},Java,$matched,$not_matched" >> $2/result.csv
	fi
	fi	
	fi
}



if [ $# -eq 4 ]
then
echo "student_id,type,matched,not_matched" > $2/result.csv
visit_and_organise unzipped $2 0 0 $3 $4
elif [ $# -eq 6 ]
then
if [ $5 = "-v" ] && [ $6 = "-noexecute" ]
then
count_test $3
visit_and_organise unzipped $2 1 1 $3 $4
fi
elif [ $# -eq 5 ]
then
if [ $5 = "-v" ]
then
count_test $3
echo "student_id,type,matched,not_matched" > $2/result.csv
visit_and_organise unzipped $2 1 0 $3 $4
elif [ $5 = "-noexecute" ]
then
visit_and_organise unzipped $2 0 1 $3 $4
fi
fi

rm -r unzipped
