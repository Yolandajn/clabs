
CC = gcc
CFLAGS = -g -Wall

debug: all
	echo '[ab]c\(.*\.\)c\?'|./hre
all: main
	mv main hre

main: main.o processor.o renderer.o renderer_stdout.o
