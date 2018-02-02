#
#PXT Microbit Makefile 
#

.PHONY: all install setup clean
all: 
	pxt install
	pxt build

install: 
	pxt deploy

test:
	pxt test
	
clean:
	pxt clean

setup:
	pip3 install --upgrade cryptography
	pxt init
	pxt add cpp

env:
	pxt target microbit
	mkdir project && cd project 
