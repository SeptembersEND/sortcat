#!/bin/sh

ls *.c */*.c | entr -s 'clear ; ./scripts/build.sh && ./a.out'
