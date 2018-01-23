#!/bin/bash 

if [ ! -d "tests" ]; then
	mkdir tests
	mkdir tests/code
	mkdir tests/mem
fi

NAMES=(gophers binary2 lorum stone woods);

total=0
for ((i=0; i<5; i++)) do
	name=${NAMES[i]}
	valgrind --log-file="tests/mem/$name.ch" ./huff -c ../char_based_header/$name.hch tests/code/$name.code tests/$name.ch 
	grep -q "ERROR SUMMARY: 0 errors" tests/mem/$name.ch
	ret=$?
	total=$((total+$ret))
	if [ $ret != 0 ]; then
		echo "Memory issue for $name char header"
	fi
	diff ../original/$name tests/$name.ch
	total=$((total+$?))
	diff ../code/$name.code tests/code/$name.code
	total=$((total+$?))
	valgrind --log-file="tests/mem/$name.bit" ./huff -b ../bit_based_header/$name.hbt tests/$name.bit
	grep -q "ERROR SUMMARY: 0 errors" tests/mem/$name.bit
	ret=$?
	total=$((total+$ret))
	if [ $ret != 0 ]; then
		echo "Memory issue for $name bit header"
	fi
	diff ../original/$name tests/$name.bit
	total=$((total+$?))
done

echo "$((25-total)) of 25 tests passed."
rm -R tests
