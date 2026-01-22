#!/bin/sh

ls *.c */*.c | entr -s 'clear ; cc main.c -lm -o a.out && ./a.out'
