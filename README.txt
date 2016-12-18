INTRODUCTION

Nuke Snake is a classic Macintosh game by David Riggle, now cloned to run on Linux, Windows, and many other platforms.  It is a cross between "snake" games and "tank" games.  Players control nuclear-powered snakes and try to destroy each other while avoiding obstacles.

Note:  This is a pre-alpha version, so it is very much incomplete.


INSTALLATION

Nuke Snake is cross-platform; the files in 'engine' are constant throughout all the ports, but other source files differ.  The 'allegro' folder is for PC (Linux, Windows, Mac OS X) while the 'tigcc' folder is for the TI-89 graphing calculator.

Currently, Nuke Snake only runs in the game folder where 'nukesnake.dat' and 'wav' are (unless you are using the tigcc version).

Linux:

To build Nuke Snake for Linux, you need to install the Allegro Game Programming library development package.  If you are running a major Linux distribution like Fedora, Ubuntu, or Debian, the package manager can be used to install this.  On Fedora:

sudo yum install allegro-devel

On Ubuntu / Debian Etch (I'm not sure about Ubuntu):

sudo apt-get install liballegro-dev

If neither of these options exist, you may need to download, compile, and install the tarball manually.

After installing the Allegro development package, open the folder 'allegro' in this source archive and type `make`.  To run, type ./nukesnake in the same directory where 'nukesnake.dat' and 'wav' are.  Enjoy!

Windows:

I use Dev-C++ to compile Nuke Snake for Windows.  It can be obtained at http://www.bloodshed.net/devcpp.html .  After installing this, obtain the Allegro DevPack for Dev-Cpp (Google search "Allegro Dev-Cpp" or try the URL http://devpaks.org/details.php?devpak=1 ) and use the PackMan.exe program in C:\Dev-Cpp to install it.

You can download the Windows source at http://www.funsitelots.com/nukesnake .  After downloading the zip, open nukesnake.dev and build.  It should work just fine.

If that doesn't work (or you can't find any zip at funsitelots.com), you'll have to use these sources to build it for Windows.  Duplicate the folder 'allegro' making sure ai.c, ai.h, game.c, game.h, layout.c, and layout.h are not 0 bytes (Windows doesn't like symbolic links).  If they are, copy these files from the 'engine' folder to your 'allegro' duplicate.  Open Dev-Cpp and create a new project.  In the Multimedia tab, there should be Allegro options.  Pick the statically-linked one, check "C" instead of "C++", and erase the default "main.c" that appears.  Add all the source files in the 'allegro' duplicate.  Now you should be able to build.  If you end up with warnings or errors like (uint does not exist), you will have to use your C knowledge to correct them (or contact me for support; see e-mail address below).

Mac OS X:

I currently do not have a Mac OS X to try this on; I would imagine it would involve downloading/installing X-Code, then downloading an Allegro development package for it.  Then you would type `make` in the 'allegro' folder to build and ./nukesnake to run.


LICENSE

Nuke Snake is free software; both code and media are MIT-licensed.


AUTHORS

David Riggle - Original concept and game, media.  Some code from his original game was converted from Pascal to C almost line-by-line for this game, too.
Joey Adams (joeyadams3.14159@gmail.com) - Engine code for Linux (Allegro) and the TI-89 (TIGCC).
