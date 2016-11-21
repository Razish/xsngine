#!/bin/bash

if [ $0 != "./scripts/build.sh" ]; then
	echo 'must be run from project root :( - try' $IDEALEXEC
	exit 1
fi

scons -Q debug=0 force32=0 tools=default --directory=../src
