#!/bin/sh
svn up
cd src
make clean
make
cd ..
