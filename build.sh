#!/bin/bash
rm main
gcc main.c -g -L domino.c -o main

if [ $# -eq 1 ] && [ $1 = "0" ]
then
	./main
elif [ $# -eq 1 ] && [ -r $1 ]
then
	./main < $1
else
	echo "To execute program type ./main"
fi
