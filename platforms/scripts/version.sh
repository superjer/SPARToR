#!/bin/bash
sed -i 's/define VERSION "[0-9.]*"/define VERSION "'`date +0.%y.%m.%d`'"/' engine/main.h
