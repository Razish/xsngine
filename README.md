# xsngine [![Build Status](https://travis-ci.org/Razish/xsngine.svg?branch=master)](https://travis-ci.org/Razish/xsngine) #
A simple engine written in C++ using SDL and OpenGL.  
Built for high performance, low-latency games using modern rendering techniques.  
Written with native execution on 32/64 bit Windows, Linux and Mac in mind.

Don't expect anything great, unless you're willing to make it =]

# Core contributors #

* Kieren "Razish" McDevitt (maintainer, designer, lead programmer)
* AstralSerpent (general programming)
* Alex "Xycaleth" Lo (lead graphics programming)

# Contributing #

Pull requests may be accepted provided they /try/ to follow the coding style guidelines (see CODE_STYLE_CONVENTIONS)  
Please discuss with a project maintainer before making any major changes to avoid wasted effort =]  
All code must be compatible with the MIT licence (see LICENSE)

# Related Repos #
* [xsngine-assets](https://github.com/Razish/xsngine-assets)
* [TODO: xsngine-tools](https://github.com/Razish/xsngine-tools)

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
Install Visual Studio 2013 Community Edition or later for C++11 features.  
All projects use scons under the hood, and there are project files available for VS2013, Codelite and Code::Blocks.  
You can also run a build from the command-line with `scons -Q` from within the `src` directory.

## Linux ##

You will require libsdl2-dev, libglu1-mesa-dev, libfreetype6-dev and libpng16-dev (which is not available in the main repositories as of Ubuntu 14.10)  
libfreetype6-dev depends on libpng12-dev, so you will have to install that first, and then install libpng16-dev (dimensio/libpng16-deb) with the following method:

```bash
$ git clone https://github.com/dimensio/libpng16-deb.git && cd libpng16-deb
$ sudo apt-get install bundler curl
$ bundle install
$ make
$ sudo dpkg -i ./*.deb
```

To compile, run the following  
```bash
$ cd src
$ scons -Q
```

## Mac ##

You will need to install Apple Developer Tools from the App Store, then use HomeBrew/brew to install git, python and scons.  
As Mac and Linux both have a UNIX environment + tools, the process between the two is much the same.


# Debugging #
All the provided project files assume there is an executable file named `xsngine` in the current working directory.  
Because different platforms will have different binary names, and these tools are used on all platforms, it is recommended to create a symlink from `xsngine.exe` to e.g. `xsn.x86.exe` - On Windows you will require the Win32 Symlink Extensions.  
On Windows, you may also want to symlink SDL2.dll to the correct one for your architecture, e.g. src/SDL2.dll -> bin/SDL2x86.dll

Run the binary with `+set com_path ../bin` to load assets correctly. See [xsngine-assets](https://github.com/Razish/xsngine-assets) for these files.
