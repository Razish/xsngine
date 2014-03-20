#!/bin/bash

# fetch arguments
ARGS=($@)
ARGSLEN=${#ARGS[@]}

# options
DEBUG=0
ANALYSE=0
FORCE32=0
COMPILER='gcc'

for (( i=0; i<${ARGSLEN}; i++ ));
do
	case ${ARGS[$i]} in
	"debug")
		DEBUG=1
		;;
	"fastdebug")
		DEBUG=2
		;;
	"clang")
		COMPILER='clang'
		;;
	"analyse")
		ANALYSE=1
		;;
	"force32")
		FORCE32=1
		;;
	*)
		;;
	esac
done

scons debug=$DEBUG compiler=$COMPILER analyse=$ANALYSE force32=$FORCE32 >/dev/null
