# Name: Xiaoyan Xu
# Student  Number: 11229743
# NSID: xix799

CC=gcc
CFLAGS=-g
CPPFLAGS= -std=gnu90 -Wall -pedantic

build: node 
node: node.o
	$(CC) -o node node.o
node.o: node.c
	$(CC) -o node.o -c $(CFLAGS) $(CPPFLAGS) node.c -lpthread
.PHONY: clean
clean: 
	rm -f node node.o
