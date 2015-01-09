# xsngine [![Build Status](https://travis-ci.org/Razish/xsngine.svg?branch=master)](https://travis-ci.org/Razish/xsngine) #
A simple engine written in C++ using SDL and OpenGL.  
Built for high performance, low-latency games using modern rendering techniques.  
Written with native execution on 32/64 bit Windows and Linux in mind. May work on Mac, but no official support as of yet.

Don't expect anything great, unless you're willing to make it =]

# Core contributors #

* Razish (maintainer, designer)
* AstralSerpent (general)
* Xycaleth (graphics)

# Contributing #

Pull requests may be accepted provided they /try/ to follow the coding style guidelines (see CODE_STYLE_CONVENTIONS)  
Please discuss with a project maintainer before making any major changes to avoid wasted effort =]  
All code must be compatible with the MIT licence (see LICENSE)

# Compiling #

## General ##
There is a scons project capable of producing Release, Debug and Optimised-Debug builds. You can also force 32 bit builds.  
There are some project files provided for VS2013, CodeLite and Code::Blocks which just invoke scons. This is for ease of debugging.  
Steps:
* Install Python 2.7
* Install SCons (preferrably latest, at-least 2.3.0)
* Run `scons` from the `src` directory


## Windows ##

Make sure `C:\Python27` and `C:\Python27\Scripts` are in your `%PATH%` environment variable.  
Install Visual Studio 2013 or later for C++11 features.  
You can try to use VS2010 but it is unsupported, and you may have to patch it with `msinttypes` (first google result)

## Linux ##

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
```

## Mac ##

There is currently no support for Mac, though xsngine aims to be platform-agnostic as can be. 
The scons project may work. The code will likely work.

Feel free to contribute for Mac support, preferrably via scons (or rip out scons if you can provide another cross-platform build tool solution)


# Debugging #
All the provided project files assume there is an executable file named `xsngine` in the current working directory.  
Because different platforms will have different binary names, and these tools are used on all platforms, it is recommended to create a symlink from `xsngine` to e.g. `xsn.x86.exe` - On Windows you will require the Win32 Symlink Extensions

Run the binary with `+set com_path ../bin` to load assets correctly. See [xsngine-assets](https://github.com/Razish/xsngine-assets) for these files.
