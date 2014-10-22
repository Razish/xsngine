#!/bin/sh

gdb -ex=r --args xsn.`uname -m` +set com_path ../bin $*
