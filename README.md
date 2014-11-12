# xsngine #

A simple engine written in C++ using SDL and OpenGL.
Built for high performance, low-latency games using modern rendering techniques.
Written with native execution on 32/64 bit Windows and Linux in mind. May work on Mac, but no official support as of yet.

Don't expect anything great, unless you're willing to make it =]

# Core contributors #

* Razish (maintainer)
* Xycaleth

# Contributing #

Pull requests may be accepted provided they /try/ to follow the coding style guidelines (see CODE_STYLE_CONVENTIONS)
Please discuss with a project maintainer before making any major changes to avoid wasted effort =]
All code must be compatible with the MIT licence (see LICENSE)

# Compiling #

## Windows ##

There is a pre-built solution for Visual Studio 2010 capable of producing x86 and x64 builds with Debug and Release configurations
You can also use scons, if you have Python 2.7 installed. I recommend using msys/git bash
```bash
$ cd src
$ python.exe /c/Python27/scripts/scons.py -Q
```

## Linux ##

Linux is currently using a scons solution capable of producing i386 and x86_64 builds with Debug and Release configurations.
There is also a Code::Blocks project which invokes scons.

You will require libfreetype6-dev and libpng16-dev (which is not available in the main repositories as of 14.10)
Beware a dependency issue for libfreetype6-dev, which requires libpng12, you must install libpng16-dev from [dimensio/libpng16-deb](https://github.com/dimensio/libpng16-deb)

```bash
$ git clone https://github.com/dimensio/libpng16-deb.git && cd libpng16-deb
$ sudo apt-get install bundler curl
$ bundle install
$ make
$ dpkg -i ./*.deb
```

```bash
$ cd src
$ scons -Q
$ ./xsn.x86_64 +set com_path ../bin
```

## Mac ##

There is currently no support for Mac, though xsngine aims to be platform-agnostic as can be.
The scons project may work. The code will likely work.

Feel free to contribute for Mac support, preferrably via scons (or rip out scons if you can provide another cross-platform build tool solution)
