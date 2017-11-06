
COMPILING:
make

EXTRA COMPILING INFORMATION
The makefile should determine if the system you are compiling on has freetype2 installed on your system.
This has been tested on a linux machine with and without freetype and the program should still compile either way
If the system doesn't have freetype the game will just compile and the font rendering just won't display.

If for some reason the makefile doesn't work you can still compile the application like you would any other OpenGL application (the code looks for the preprocessor definition -DUSEFREETYPE to determine which section of code to compile with).

If the system doesn't have freetype and you would like to compile it with freetype, install the library libfreetype6-dev.
