# INTRODUCTION

Nuke Snake is a cross between "snake" games and "tank" games.  Players control nuclear-powered snakes and try to destroy each other while avoiding obstacles.

Nuke Snake was originally written for classic (pre-OS X) Macintosh by David Riggle.  This is a clone with some added features and support for multiple platforms.

Note that this is still in "alpha".  It is mostly feature-complete and entirely playable, but there are some missing features.

# INSTALLATION

Nuke Snake is cross-platform; the files in 'engine' are constant throughout all the ports, but other source files differ.  The 'allegro' folder is for PC (Linux, Windows, Mac OS X) while the 'tigcc' folder is for the TI-89 graphing calculator.

Currently, Nuke Snake only runs in the game folder where 'nukesnake.dat' and 'wav' are (unless you are using the tigcc version).

The "qt", "ecere", and "engine2" folders contain unfinished code.

## Linux

To build Nuke Snake for Linux, you need to install the Allegro Game Programming library development package, version 4.  If you are running a major Linux distribution like Fedora, Ubuntu, or Debian, the package manager can be used to install this.  On Ubuntu:

    sudo apt-get install liballegro4-dev

and on Fedora:

    sudo yum install allegro4-devel

If neither of these options exist, you may need to download, compile, and install the tarball manually.

After installing the Allegro development package, open the folder 'allegro' in this source archive and type `make`.  To run, type ./nukesnake in the same directory where 'nukesnake.dat' and 'wav' are.  Enjoy!

## Windows

In the past, I compiled Nuke Snake for Windows using [Dev-C++](http://www.bloodshed.net/devcpp.html).  To do so, download the [Allegro DevPack for Dev-Cpp](http://devpaks.org/details.php?devpak=1) and use the PackMan.exe program in C:\Dev-Cpp to install it.  You may have to fix warnings or errors like "uint does not exist".

# LICENSE

Nuke Snake is free software; both code and media are MIT-licensed.

# AUTHORS

 * David Riggle - Original concept and game, media.  Some code from his original game was converted from Pascal to C almost line-by-line for this game, too.

 * Joey Adams (joeyadams3.14159@gmail.com) - Engine code for Linux (Allegro) and the TI-89 (TIGCC).

