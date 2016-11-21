#!/bin/bash

#if [ $UID -ne 0 ]; then
#	echo 'must be run as root :('
#	exit 1
#fi

IDEALEXEC=./scripts/depends.sh
if [ $0 != "./scripts/depends.sh" ]; then
	echo 'must be run from project root :( - try' $IDEALEXEC
	exit 1
fi

if [ ! -f /etc/debian_version ]; then
	echo 'we are not debian-like enough :('
	exit 1
fi

# libfreetype6-dev requires libpng12-dev
# we eventually want libpng16-dev, but this is incompatible with libpng12-dev
sudo apt-get install gcc libsdl2-dev libglu1-mesa-dev libfreetype6-dev

# see if we need libpng16-dev
apt-cache policy libpng16-dev | grep Installed > /dev/null
if [ $? -eq 1 ]; then
	# install libpng16-dev from source
	sudo apt-get install git
	PNGDIR=libpng16-deb
	git clone https://github.com/dimensio/libpng16-deb.git $PNGDIR && cd $PNGDIR
	sudo apt-get install bundler curl
	bundle install
	make
	sudo dpkg -i ./*.deb
fi
