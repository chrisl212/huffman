#!/bin/bash 

./huff -c ../char_based_header/gophers.hch tests/code/gophers.code tests/gophers.ch
./huff -c ../char_based_header/binary2.hch tests/code/binary2.code tests/binary2.ch
./huff -c ../char_based_header/lorum.hch tests/code/lorum.code tests/lorum.ch
./huff -c ../char_based_header/stone.hch tests/code/stone.code tests/stone.ch
./huff -c ../char_based_header/woods.hch tests/code/woods.code tests/woods.ch

diff ../original/gophers tests/gophers.ch
diff ../original/binary2 tests/binary2.ch
diff ../original/lorum tests/lorum.ch
diff ../original/stone tests/stone.ch
diff ../original/woods tests/woods.ch

diff ../code/gophers.code tests/code/gophers.code 
diff ../code/binary2.code tests/code/binary2.code 
diff ../code/lorum.code tests/code/lorum.code 
diff ../code/stone.code tests/code/stone.code 
diff ../code/woods.code tests/code/woods.code 

./huff -b ../bit_based_header/gophers.hbt tests/gophers.bit
./huff -b ../bit_based_header/binary2.hbt tests/binary2.bit
./huff -b ../bit_based_header/lorum.hbt tests/lorum.bit
./huff -b ../bit_based_header/stone.hbt tests/stone.bit
./huff -b ../bit_based_header/woods.hbt tests/woods.bit

diff ../original/gophers tests/gophers.bit
diff ../original/binary2 tests/binary2.bit
diff ../original/lorum tests/lorum.bit
diff ../original/stone tests/stone.bit
diff ../original/woods tests/woods.bit
