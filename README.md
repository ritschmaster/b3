__b__(aeck's implementation of i) __3__ (for windows)

# Dependencies

* [MinGW](http://mingw.org/)
* [Collections-C](https://github.com/srdja/Collections-C)
* [w32bindkeys](https://github.com/ritschmaster/w32bindkeys)

# Compiling

# On Windows using Msys2

You will need a functioning MinGW environment. This sections describes the compilation using [Msys2](https://www.msys2.org/).

First install the dependencies:

    pacman -S gcc make cmake automake autoconf

Then compile and install Collections-C:

    wget https://github.com/srdja/Collections-C/archive/master.zip
    unzip master.zip
    cd Collections-C-master/
    sed -i s/add_subdirectory\(test\)//g CMakeLists.txt # Testing is not needed
    mkdir build
    cd build
    cmake -DCMAKE_INSTALL_PREFIX=/usr ..

Then compile and install w32bindkeys. Please have a look at its [README](https://github.com/ritschmaster/w32bindkeys/blob/master/README.md).

Then compile b3:

    ./configure
    make

Afterwards you may install b3 - depending on your MinGW environment - by performing:

    make install

## On Fedora Linux (64 bit)

First install MinGW:

    sudo dnf install -y mingw64-gcc mingw64-gcc-c++ mingw64-winpthreads-static cmake automake autoconf autoconf-archive libtool

Then compile and install Collections-C:

    wget https://github.com/srdja/Collections-C/archive/master.zip
    unzip master.zip
    cd Collections-C-master/
    sed -i s/add_subdirectory\(test\)//g CMakeLists.txt # Testing is not needed
    mkdir build
    cd build
    mingw64-cmake ..
    sudo make install
    sudo cp /usr/x86_64-w64-mingw32/sys-root/mingw/lib/pkgconfig/collectionc.pc /usr/lib64/pkgconfig

Then compile and install w32bindkeys. Please have a look at its [README](https://github.com/ritschmaster/w32bindkeys/blob/master/README.md).

Then compile b3:

    mingw64-configure
    make



