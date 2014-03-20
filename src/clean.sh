#!/bin/bash

# fetch arguments
ARGS=($@)
ARGSLEN=${#ARGS[@]}

# options
DEBUG=0
FORCE32=0

for (( i=0; i<${ARGSLEN}; i++ ));
do
	case ${ARGS[$i]} in
	"debug")
		DEBUG=1
		;;
	"fastdebug")
		DEBUG=2
		;;
	"force32")
		FORCE32=1
		;;
	*)
		;;
	esac
done

scons debug=$DEBUG force32=$FORCE32 -c >/dev/null
