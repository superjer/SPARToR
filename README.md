SPARToR Network Game Engine
===========================

by SuperJer

<http://www.superjer.com>

The engine is in early development. Two in-development games are included: The Dead Kings and McDiddy's.

Build on Linux
--------------
1. Install the following, using your package manager:
    * gcc
    * libsdl2-dev
    * libsdl2-image-dev
    * libsdl2-net-dev
    * libglew-dev
2. Run `make`

Build on Windows
----------------
1. Install msysGit (bottom of page)
    * <https://msysgit.github.io/>
2. Clone SPARToR inside msysGit:
    * git clone https://github.com/superjer/SPARToR
    * cd SPARToR
3. Have Git get the Windows dependencies for you:
    * git submodule init -- platforms/win32
    * git submodule update
4. Run `make`

Build on OSX
------------
1. Get Xcode, or just "Command Line Tools for Xcode" which is sufficient and MUCH smaller
    * <https://developer.apple.com/downloads/index.action>
2. Install the dev libraries for SDL 2, SDL_net 2 and SDL_image 2 in /Library/Frameworks
    * <https://www.libsdl.org/download-2.0.php>
    * <https://www.libsdl.org/projects/SDL_net/>
    * <https://www.libsdl.org/projects/SDL_image/>
3. Run `make`
