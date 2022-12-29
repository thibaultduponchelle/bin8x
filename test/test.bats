setup() {
    DIR="$( cd "$( dirname "$BATS_TEST_FILENAME" )" >/dev/null 2>&1 && pwd )"
    ROOT="$DIR/.."
    #PATH="$ROOT:$PATH"
    cd $ROOT
    make
    cd $DIR
}


@test "Very basic" {
    ../bin8x -i test.bin -o TEST.83p
}

@test "This test displays help" {
    ../bin8x -h | grep "Kuipers"
    ../bin8x -u TEST.83p test.bin -h | grep "Coosemans"
    ../bin8x -help | grep "Hoffmann"
}


@test "Tests basic" {
    ../bin8x test.bin
    ../bin8x -i test.bin
    ../bin8x -o test.83p
    ../bin8x -i test.bin -o TEST.83p
    ../bin8x -i test.bin -o TEST.83p -q -u
}

@test "Tests long option" {
    ../bin8x test.bin
    ../bin8x -input test.bin
    ../bin8x -output test.83p
    ../bin8x -input test.bin -output TEST.83p
    ../bin8x -output TEST.83p test.bin  -verbose
    ../bin8x -output TEST.83p test.bin  -executable
    ../bin8x -input test.bin -output TEST.83p -unsquish -unprotected
}

@test "Test autoselect" {
    ../bin8x test.bin TEST.83p
    ../bin8x TEST.83p test.bin
    ../bin8x TEST.8xp test.bin
    ../bin8x TEST.82p test.bin
}

@test "Test more dangerous" {
    ../bin8x test.bin TEST.83p -u -q -u
    ../bin8x -q -l -u test.bin TEST.83p
}

@test "Test some options between input and output" {
    ../bin8x -q TEST.83p -u test.bin
    ../bin8x -q TEST.83p -u test.bin
    ../bin8x -q -u TEST.83p test.bin
}

@test "Test name too long/lowercase" {
    ../bin8x -q -u TEST.83p test.bin -n TESTTOOlONG
    ../bin8x -q -u TEST.83p test.bin -n uppercase
    ../bin8x -l -q -u TEST.83p test.bin -n lowercase
}

@test "Some other tests" {
    ../bin8x -u TESTTOOLONG.83p test.bin -q
    ../bin8x TEST.83p test_too_long.bin -q -u
    ../bin8x TEST.83p test.bin -q -u
}

@test "Test should fail" {
    #../bin8x -v TEST.83p __temp__.bin -q -u
}

@test "Test -n" {
    # Compare file generated with explicit -n name
    # Versus name taken from filename
    ../bin8x -o TESTTOOLONG.83p -i test.bin
    ../bin8x -o B.83p -i test.bin -n TESTTOOLONG
    diff TESTTOOLONG.83p B.83p
}

@test "Check \0 padding (v2.1)" {
    ../bin8x -o PAD.83p -i test.bin
    ../bin8x -o PADN.83p -i test.bin -n PAD
    diff PADN.83p PAD.83p
}

@test "Compare with bin2var" {
    # Compare with another converter
    # echo "COMPARE WITH BIN2VAR"
    # ./bin2var test.bin PAD.83p
    # mv PAD.83p B2V.83p
    # ./bin8x -o PAD.83p -i test.bin
}

