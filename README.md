# b3

![GitHub](https://img.shields.io/github/license/ritschmaster/b3)
[![Telegram chat](https://img.shields.io/badge/chat-%40b3__dev-%2335ADE1?logo=telegram)](https://t.me/b3_dev)
![GitHub All Releases](https://img.shields.io/github/downloads/ritschmaster/b3/total)
[![Donate with Bitcoin](https://en.cryptobadges.io/badge/micro/3DF2eTL9KSndqbuQTokWvTbUpDNQ6RuwxU)](#donations)

__b__(aeck's implementation of i) __3__ (wm for Windows)

## Using b3

### Using binary releases

1. Head over to the [Release](https://github.com/ritschmaster/b3/releases) page and download the latest binary zip file.
2. Decompress it somwhere
3. Copy the `example.config` to `bin` (See [FAQ](#faq) if you want to personalize the configuration))
4. Execute `b3.exe`

### Using source code releases

1. Head over to the [Release](https://github.com/ritschmaster/b3/releases) page and download the latest source zip file.
2. Decompress it somwhere
3. See [Compiling](#Compiling)
4. Copy the `example.config` to `bin` (See [FAQ](#faq) if you want to personalize the configuration))
5. Execute `b3.exe`

## FAQ

### What features of i3 are currently supported?

See [here](https://github.com/ritschmaster/b3/blob/master/CompatibilityList.md).

### How does b3 integrate into Windows?

* Your default Windows expierence stays the same. b3 lives within the Microsoft
  default Explorer shell and just adds functionalities you know and love of i3.
* Stuff like <kbd>Alt</kbd> + <kbd>Tab</kbd> is still avialable
* Switching between windows in the plain Windows way will be automatically
  recognized by b3 (e.g. switch to workspace of that window).
* The Windows 10 desktop are still available but useless when b3 is running.
* It is possible to rebind almost any key combination. <kbd>Win</kbd> + <kbd>h</kbd>, <kbd>j</kbd>, <kbd>k</kbd>, <kbd>l</kbd> is supported (for <kbd>l</kbd> see [here](https://github.com/ritschmaster/w32bindkeys#i-want-to-remap-the-win-key))

### What are the project's goals?

1. Be as i3 like as possible while integrating into default Windows as easily as possible.
2. Being stable is more important than having more features

### I need a default configuration

A default config file is included in the [release zip](https://github.com/ritschmaster/b3/releases). There is also an example configuration available at `tests/full.config` (please place it as `config` in the same directory as `b3.exe` to use it).

For more information on where the configuration file can be placed please see the included man page. In a release it can be viewed using

    man /path/b3/share/man/man1/b3.1

### Are there known limitations?

Yes, please see [here](https://github.com/ritschmaster/w32bindkeys#are-there-known-limitations).

## Compiling

### Dependencies

* [MinGW](http://mingw.org/)
* [Collections-C](https://github.com/srdja/Collections-C)
* [w32bindkeys](https://github.com/ritschmaster/w32bindkeys)

### On Windows using Msys2

You will need a functioning MinGW environment. This sections describes the compilation using [Msys2](https://www.msys2.org/).

First install the dependencies:

    pacman -S make cmake automake autoconf mingw-w64-x86_64-gcc libtool bison flex mingw64/mingw-w64-x86_64-pcre

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

If you are unsatisfied with b3 you may uninstall it again:

    make uninstall

### On Fedora Linux (64 bit)

First install MinGW:

    sudo dnf install -y mingw64-gcc mingw64-gcc-c++ cmake automake autoconf autoconf-archive libtool bison flex mingw64-pcre

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

    export PKG_CONFIG_PATH=/usr/x86_64-w64-mingw32/lib/pkgconfig/
    ./configure --host=x86_64-w64-mingw32
    make

## Version scheme

The version scheme of w32bindkeys is as follows: x.y.z

* x is the stage:
  * < 0 is an unstable release
  * \> 0 is a stable release
* y incidates changes to the API or the configuration file functionalities.
* z indicates additions without breaking any compatibility.

## Author

Richard Bäck <richard.baeck@mailbox.org>

## License

MIT License

## Donations

Show me that you really like it by donating something. Thanks for any amount!

My Bitcoin address: 3DF2eTL9KSndqbuQTokWvTbUpDNQ6RuwxU
