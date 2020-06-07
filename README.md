# b3

[![Join the chat at https://gitter.im/b3wm/community](https://badges.gitter.im/b3wm/community.svg)](https://gitter.im/b3wm/community?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

__b__(aeck's implementation of i) __3__ (for windows)


## Compiling

### Dependencies

* [MinGW](http://mingw.org/)
* [Collections-C](https://github.com/srdja/Collections-C)
* [w32bindkeys](https://github.com/ritschmaster/w32bindkeys)

### On Windows using Msys2

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

    ./autogen.sh
    make

Afterwards you may install b3 - depending on your MinGW environment - by performing:

    make install

### On Fedora Linux (64 bit)

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

## Using b3

### Using precompiled releases

1. Head over to the [Release](https://github.com/ritschmaster/b3/releases) page and download the latest zip file.
2. Decompress it somwhere
3. Execute `b3.exe`

## Version scheme

The version scheme of w32bindkeys is as follows: 1.2.3

1. Stages:
 0. Alpha
 1. Beta
 2. For Production use
2. Releases breaking compatibility of its API or the configuration file.
3. Collection of changes not breaking any compatibility.

## Author

Richard Bäck <richard.baeck@mailbox.org>

## License

MIT License
