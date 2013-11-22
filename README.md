# xsngine #

A simple engine written in C++ using SDL and OpenGL.  
Built for high performance, low-latency games using modern rendering techniques.  
Written with native execution on 32/64 bit Windows and Linux in mind. May work on Mac, but no official support as of yet.

Don't expect anything great, unless you're willing to make it =]

# Core contributors #

* Razish (maintainer)

# Contributing #

Pull requests may be accepted provided they /try/ to follow the coding style guidelines (see CODE_STYLE_CONVENTIONS)  
Please discuss with a project maintainer before making any major changes to avoid wasted effort =]  
All code must be compatible with the MIT licence (see LICENSE)

# Compiling #

## Windows ##

There is a pre-built solution for Visual Studio 2010 capable of producing x86 and x64 builds with Debug and Release configurations

## Linux ##

Linux is currently using an ugly scons solution capable of producing i386 and x86_64 builds with Debug and Release configurations

```bash
$ cd src
$ scons debug=1
$ ./xsn.x86_64 +set com_path ../bin
```

## Mac ##

There is currently no support for Mac, though xsngine aims to be platform-agnostic as can be.  
The scons project may work. The code will likely work.

Feel free to contribute for Mac support, preferrably via scons (or rip out scons if you can provide another cross-platform build tool solution)
