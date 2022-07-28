# libiosexec
A shim library that both works to allow shell scripts to execute correctly on iOS, and provides a framework for true rootless support.

## Installation
A build of ``libiosexec`` is provided on [Procursus](https://github.com/ProcursusTeam/Procursus), which you can install if your device is already setup with the build system.

Alternatively, you can compile ``libiosexec`` on your device using Git and Make. Run the commands below to build the project

    make install DESTDIR="/install/path"

There are other variables you can specify when building; checkout the Makefile.

## Usage
There's not that much to say here; simply define the symbols in your project/script
    
    /* Wrapper functions to make iOS shells scripts function correctly */
    #include <libiosexec.h>

<sub>Note that this implementation follows FreeBSD/Linux behavior of not splitting the argument passed to the shebang, unlike macOS which does.</sub>
