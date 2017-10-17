#! /bin/bash
java -cp "jna-4.5.0.jar:." absolut.acc.Main

python3 ../../../testPython/test.py $1
