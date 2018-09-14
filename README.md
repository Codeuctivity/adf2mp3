[![Build status](https://ci.appveyor.com/api/projects/status/cbw5itqw2niv2pqs?svg=true)](https://ci.appveyor.com/project/stesee/mp4boxsharp)

#ADF2MP3 - A command line tool for converting GTA Vice City ADF audio files to MP3

----

##About:

[*Grand Theft Auto - Vice City*][1] radio stations and soundtracks are stored in a proprietary file format,
with the `.adf` extension. This file format is a MP3 that had each byte *XORed* with
the decimal constant `34`. The `adf2mp3` command line tool allows the decoding of Vice City
ADF files to MP3 files that can be played outside the game.

##Usage:

This tool can be used from the command prompt as such:

    $ ./adf2mp3 flash.adf

Looks for the file "flash.adf" in the current directory and writes a
file named "flash.mp3" to the current directory as its output. You can
also provide an explicit output filename as the third parameter:

    $ ./adf2mp3 flash.adf flash_fm_radio.mp3

To print basic help and tool info, run:

    $ ./adf2mp3 --help

##Directory Structure:

    +-adf2mp3/
     |
     +-bin/                      => Contains a prebuilt binary for Windows (tested on Windows 8).
     |
     +-ports/                    => Ported versions of the original C++ code. Currently, I've written a Rust version of the tool that works just as well.
     |
     +-adf2mp3.cpp               => The single source code file for the application.
     |
     +-makefile                  => A Unix makefile that builds adf2mp3.cpp using a C++11 compiler (tested on Mac OS X / Clang).
     |
     +-adf2mp3.vcxproj & friends => Visual Studio project files...

##License:

This code is public domain. I claim no copyrights over it.
No warranty is offered or implied; use it at your own risk.

[1]: http://en.wikipedia.org/wiki/Grand_Theft_Auto%3A_Vice_City

## Known Locations of radio station soundtracks

### Grand Theft Auto - Vice City (Steam Version)

+-C:\Program Files (x86)\Steam\steamapps\common\Grand Theft Auto Vice City\Audio
 +-EMOTION.adf
 |
 +-ESPANT.adf
 |
 +-FEVER.adf
 |
 +-FLASH.adf
 |
 +-KCHAT.adf
 |
 +-VCPR.adf
 |
 +-VROCK.adf
 |
 +-WAVE.adf
 |
 +WILD.adf