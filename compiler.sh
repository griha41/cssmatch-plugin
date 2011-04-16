#!/bin/sh
make release 2>debug_make.txt
more debug_make.txt | grep "\(\(e|E\)rror\|\(e|E\)rreur\)"
