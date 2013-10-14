#!/bin/sh
make
./testHunter | perl trans.pl 
