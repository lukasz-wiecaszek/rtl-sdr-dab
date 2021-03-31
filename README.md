# rtl-sdr-dab

RTL SDR DAB receiver based on gnuradio infrastructure.
It uses gr-bad module (https://github.com/lukasz-wiecaszek/gr-bad)
to accomplish its work so as well as gr-bad it also only supports DAB mode I.

## How to build this application

To build this project you will definitely need some external packages.
The first and the most obvious one is the gnuradio (https://www.gnuradio.org/).
And it needs to be version 3.8. You can install it from sources
or by fetching the distribution's packages. For example:
```
  $ sudo apt install gnuradio
```

The second required package is the gr-bad module. Just go to the webpage
https://github.com/lukasz-wiecaszek/gr-bad and follow the instructions.

There are also other dependencies but they will be fetched as git submodules.
And then as with every cmake installation, just type the following:

```
  $ git submodule update --init --recursive
  $ mkdir build
  $ cd build
  $ cmake ..
  $ make
```
