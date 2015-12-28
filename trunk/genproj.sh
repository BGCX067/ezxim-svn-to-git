#!/bin/sh
progen -o ezxim.pro
cat "ezxim.pro.aux" >> ezxim.pro
tmake ezxim.pro > Makefile