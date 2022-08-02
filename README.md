dbhandler
=========


Building using XCode
--------------------

On OSX, use XCode to build dbhandler.


Building using Visual Studio
----------------------------

On Windows, use Visual Studio to build dbhandler.


Building everywhere using automake
----------------------------------

Building dbhandler requires a number of software tools and libraries
to be installed first (possibly including their -dev or -devel)
packages:

    autoconf
    automake
    gettext
    intltool
    libtool

    UnixODBC
    gtk+ 3.0
    libpq (postgresql client library)
    mariadb/mysql
    wxWidgets

Note that the following build instructions use `make` options using
`nproc` to make the builds run to use as many CPU corse in parallel as
possible.  However, those options are GNU make options and are using
GNU coreutils, so you may skip them if your system does not have
those.

dbhandler requires a relatively recent version of wxWidgets, so you
might need to install wxWidgets yourself

    cd ~/src
    git clone --recurse-submodules https://github.com/wxWidgets/wxWidgets.git
    cd wxWidgets
    ./autogen.sh
    ./configure --prefix=$PWD/_p
    make -j$(nproc) -l$(nproc)
    make -j$(nproc) -l$(nproc) install

On Linux, you will now need to tell the dynamic linker where to find
the wx* libraries as follows (other systems will have other methods to
do that):

    export LD_LIBRARY_PATH="$HOME/src/wxWidgets/_p/lib${LD_LIBRARY_PATH+":${LD_LIBRARY_PATH}"}"

It might also be useful to adapt PATH such that wx-config can be found:

    export PATH="$HOME/src/wxWidgets/_p/bin${PATH+":${PATH}"}"

If you did a custom install for other libraries such as UnixODBC, gtk+
or libpq, it might be useful to add the respective
`${libdir}/pkgconfig` directory to the `PKG_CONFIG_PATH` environment
variable in a similar way.

Then you can build (and possibly install) dbhandler itself:

    cd ~/src
    git clone https://github.com/oneeyeman1/dbhandler.git
    cd dbhandler
    autoreconf -if
    ./configure --prefix=$PWD/_p --with-wx-config=$HOME/src/wxWidgets/_p/bin/wx-config
    make -j$(nproc) -l$(nproc)
    make -j$(nproc) -l$(nproc) install

If you want to run `make distcheck`, you need to replace the above
`configure` command with one setting `DISTCHECK_CONFIGURE_FLAGS`:

    ./configure --prefix=$PWD/_p --with-wx-config=$HOME/src/wxWidgets/_p/bin/wx-config DISTCHECK_CONFIGURE_FLAGS="--with-wx-config=$HOME/src/wxWidgets/_p/bin/wx-config"
    make -j$(nproc) -l$(nproc) distcheck
