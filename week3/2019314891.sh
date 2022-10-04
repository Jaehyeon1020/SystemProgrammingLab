#!/bin/bash
make
./plusAndMinus > output.txt
diff output.txt answer.txt > result.txt
