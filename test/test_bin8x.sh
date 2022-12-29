#!/bin/bash

# This test displays help
echo "DISPLAY HELP"
./bin8x -u TEST.83p test.bin -h
./bin8x -help
./bin8x -h

# Tests basic
echo "TESTS BASIC"
./bin8x test.bin
./bin8x -i test.bin
./bin8x -o test.83p
./bin8x -o TEST.83p
./bin8x -i test.bin -o TEST.83p
./bin8x -i test.bin -o TEST.83p -q -u

# Tests long option
echo "TEST LONG OPTION"
./bin8x test.bin
./bin8x -input test.bin
./bin8x -output test.83p
./bin8x -output TEST.83p
./bin8x -input test.bin -output TEST.83p
./bin8x -output TEST.83p test.bin  -verbose
./bin8x -output TEST.83p test.bin  -executable
./bin8x -input test.bin -output TEST.83p -unsquish -unprotected

# Test autoselect_ext
echo "TEST AUTOSELECT OPTION"
./bin8x test.bin TEST.83p
./bin8x TEST.83p test.bin
./bin8x TEST.8xp test.bin
./bin8x TEST.82p test.bin

# Test more dangerous
echo "TEST MORE DANGEROUS"
./bin8x test.bin TEST.83p -u -q -u
./bin8x -q -l -u test.bin TEST.83p

# Test some options between input and output
echo "TEST OPTIONS BWETWEEN INPUT AND OUTPUT"
./bin8x -q TEST.83p -u test.bin
./bin8x -q TEST.83p -u test.bin
./bin8x -q -u TEST.83p test.bin

# Test name too long/lowercase
echo "TEST TOO LONG"
./bin8x -q -u TEST.83p test.bin -n TESTTOOlONG
./bin8x -q -u TEST.83p test.bin -n uppercase
./bin8x -l -q -u TEST.83p test.bin -n lowercase

# Some other tests
echo "SOME OTHER TESTS"
./bin8x -u TESTTOOLONG.83p test.bin -q
./bin8x TEST.83p test_too_long.bin -q -u
./bin8x TEST.83p test.bin -q -u

# This one should fail
echo "THIS ONE FAILS"
./bin8x -v TEST.83p __temp__.bin -q -u

# Compare file generated with explicit -n name
# Versus name taken from filename
echo "TEST WITH OR WITHOUT -n"
./bin8x -o TESTTOOLONG.83p -i test.bin
./bin8x -o B.83p -i test.bin -n TESTTOOLONG
diff TESTTOOLONG.83p B.83p

# Check \0 padding (v2.1)
echo "CHECK ZERO PADDING"
./bin8x -o PAD.83p -i test.bin
./bin8x -o PADN.83p -i test.bin -n PAD
diff PADN.83p PAD.83p

# Compare with another converter
# echo "COMPARE WITH BIN2VAR"
# ./bin2var test.bin PAD.83p
# mv PAD.83p B2V.83p
# ./bin8x -o PAD.83p -i test.bin
