[![GitHub](https://img.shields.io/badge/GitHub-2B3137?style=flat-square&logo=GitHub&logoColor=FFFFFF)](https://github.com/XPhyro/scripts)
[![Codeberg](https://img.shields.io/badge/Codeberg-2185D0?style=flat-square&logo=codeberg&logoColor=F2F8FC)](https://codeberg.org/XPhyro/scripts)
[![CodeFactor](https://www.codefactor.io/repository/github/xphyro/scripts/badge?style=flat-square)](https://www.codefactor.io/repository/github/xphyro/scripts)
[![Lines of Code](https://img.shields.io/tokei/lines/github/XPhyro/scripts?color=red&style=flat-square)](src)
[![GitHub Build Workflow Status](https://img.shields.io/github/actions/workflow/status/XPhyro/scripts/build.yml?branch=master&style=flat-square&label=build%20%26%20test)](https://github.com/XPhyro/scripts/actions/workflows/build.yml)
[![GitHub Analyse Workflow Status](https://img.shields.io/github/actions/workflow/status/XPhyro/scripts/analyse.yml?branch=master&style=flat-square&label=analyse)](https://github.com/XPhyro/scripts/actions/workflows/analyse.yml)
[![GitHub Spell Workflow Status](https://img.shields.io/github/actions/workflow/status/XPhyro/scripts/spell.yml?branch=master&style=flat-square&label=spell)](https://github.com/XPhyro/scripts/actions/workflows/spell.yml)

# scripts

This repository holds a collection of general and specific scripts, utilities
and libraries for Unix-like operating systems. Almost all scripts are written in
POSIX Shell, i.e. `sh`; and most non-script utilities and libraries are written
in C.

## Installation

To install for your user:

```sh
git submodule update --init --recursive
make install
```

To install system-wide (recommended):

```sh
git submodule update --init --recursive
sudo make install
```

You may change the installation directory by setting `$PREFIX`.

If you would like to use the provided wrapper scripts, add `$PREFIX/bin/wrapper`
to your `$PATH` with higher priority than the locations of the wrapped scripts.
Some scripts also require `$PREFIX/share/scripts/include` to be in your path.

## Uninstallation

If you installed for your user:

```sh
make uninstall
```

If you installed system-wide:

```sh
sudo make uninstall
```

You do not need to re-set `$PREFIX` while uninstalling.

If you altered your `$PATH`, you may also wish to undo it.

## Usage

See `man COMMAND`, `COMMAND -h` or `COMMAND --help`. Simple programs/scripts do
not have man pages or help dialogues, either infer usage from their names, or see
the source code.

## Notable Scripts and Utilities

- [afgrep](src/c/util/core/afgrep.c): Like `grep -F`, but supports alignment and
  offset.
- [bspwm-autokblayout](src/sh/bspwm/daemon/bspwm-autokblayout): Automatically switch between keyboard layouts for different windows in [bspwm](https://github.com/baskerville/bspwm).
- [bspwmpad](src/sh/bspwm/util/bspwmpad): Dynamic scratch pads for
  bspwm. For optimum operation, include
  `seq 0 9 | while IFS= read -r i; do bspc rule -a "*:bspwmpad$i" state=floating hidden=true; done` in your `bspwmrc`. bspwm is not required
  for operation, but the script was designed with it in mind. There is also
  [dwmpad](src/sh/.archived/dwm/util/dwmpad).
- [cast](src/cpp/util/ishell/cast.cpp): Cast data between different data types.
- [clplog](src/sh/daemon/clplog): Log clipboard history.
- [clplogynk](src/sh/hotkey/clplogynk): Use `rofi` to yank an element from the
  clipboard history.
- [contexec](src/sh/daemon/contexec): Continuously execute a file every time it
  is modified. This is especially useful if you are iteratively processing data
  using an interpreted or hot-compiled language.
- [eln](src/sh/ishell/eln): Batch edit or create symlinks.
- [expandpath](src/c/util/core/expandpath.c): Like `wordexp`, but only expands
  `~`.
- [ffmw](src/sh/softwrapper/ffmw): `ffmpeg` wrapper for common actions.
- [fmaps](src/c/util/core/fmaps.c): Map standard input per given key-value
  pairs.
- [kdialog](src/sh/wrapper/kdialog): Wrapper for KDE's `kdialog` to trick
  applications into using `lf` as the file picker.
- [latexd](src/sh/daemon/latexd): Basically `latexmk` but sucks less and is
  more minimal.
- [mapexec](src/sh/util/mapexec): Open standard input in `$EDITOR` and execute
  commands against all, modified, unmodified or wiped lines synchronously or
  asynchronously. Basically [batch](https://github.com/alexherbo2/batch), but
  more versatile.
- [maptouch](src/sh/udev/maptouch): Map touchscreen and stylus devices to the
  touch-enabled display. Best used in `.xinitrc` or as a `udev` rule. Here's an
  example rule for Microsoft Surface Pen EYU-000001 and ASUS ROG Flow X13
  GVE301RC: `ACTION=="change", KERNEL=="hid-0018:04F3:2C82.0002-battery", SUBSYSTEM=="power_supply", DRIVER=="", ENV{MAPTOUCH_ROOT_DEVNAME}="ELAN9008:00 04F3:2C82", ENV{XRH_EDP}="eDP-1", RUN+="/usr/local/bin/maptouch"`.
- [mkparent](src/c/util/sys/mkparent.c): Create the parent directories of a
  path.
- [neomuttpick](src/sh/integration/neomutt/neomuttpick): Use
  [kdialog](src/sh/wrapper/kdialog) to pick attachments in `neomutt`. Add `macro compose A "<shell-escape>neomuttpick<enter><enter-command>source /tmp/neomuttpick<enter>" "Attach With File Manager"` to your `muttrc` file to
  use.
- [notiflog](src/py/daemon/notiflog): Log notifications on the dbus interface
  `org.freedesktop.Notifications`. Supports locking logging: after initiating
  the lock, the next notification is not logged. Then it unlocks itself
  automatically. Supports blacklisting: certain notifications can be blacklisted
  and logged to a different file.
- [numsh](src/c/util/math/numsh.c): A NumPy-like interface for the `math.h` C
  standard header, enabling fast mathematical computations of an array of
  numbers in the shell.
- [rgb24togray](src/c/util/math/rgb24togray.c): Convert RGB24 images/video to
  greyscale with selectable algorithms.
- [scratchpad](src/sh/hotkey/scratchpad): Take and manage notes with `vim` and
  `dmenu`. Can be used with `dwmpad` or `bspwmpad`.
- [selfl](src/sh/util/selfl): Select a file or directory with `$MENU`.
- [shufr](src/cpp/util/core/shufr.cpp): Like `shuf -r`, but supports outputting
  unique sequences.
- [std::unordered_map](src/cpp/util/std/std::unordered_map.cpp): Wrapper for
  `std::unordered_map<std::string, std::string>` for shells.
- [std::vector](src/cpp/util/std/std::vector.cpp): Wrapper for
  `std::vector<std::string>` for shells.
- [stest](src/c/util/core/stest.c): Filter a list of files by properties. This
  is a superset of the `stest` included with suckless' `dmenu`.
- [sumsize](src/c/util/core/sumsize.c): Sum human readable or raw sizes.
- [tglapp](src/sh/hotkey/util/tglapp): Toggle an application on/off based on
  hash codes. Originally written for toggling applications with the same hotkey
  via `sxhkd`.
- [unexpandpath](src/c/util/core/unexpandpath.c): Undo `expandpath`.
- [wordexp](src/c/util/core/wordexp.c): Perform word expansion like a POSIX
  shell.
- [xins](src/sh/util/xins): Like `xargs` but for standard input.

## Environment Variables

Some scripts use optional or mandatory environment variables for applications.
Assign either `bspwmpad` or `dwmpad` to `$PAD`, `rofi -dmenu` or `dmenu` to
`$MENU`, and the respective application will be used. Other used variables
include: `$BROWSER`, `$EDITOR`, `$OPENER`, `$PAGER`, `$SHELL`, `$TERMINAL`,
`$VPN`, `$_BROWSER`. Some scripts will not respect these as they use
application-specific options, such as `rofi -font` or `st -d`.

In all shell scripts, if you set `$SHELL_VERBOSE` to greater than 0, `set -x` is
executed and all executed commands are printed to standard error.

## Caveats

- Some scripts assume that some of the other scripts are in your `$PATH`.
- None of the scripts pre-check for the availability of their dependencies.
- In some scripts, GNU extensions like `sed -i` or `strcasestr` are used.
- Some scripts were written with only Linux in mind, although they may be
  compatible with other Unixes and Unix-likes, or may be ported with minimal
  modification.
- All scripts/programs are only tested on Linux.
- C code are only tested with GCC & clang, and C++ code are only tested with
  GCC. Most C/C++ programs should be compatible with any POSIX/ISO-conforming
  compilers. Some programs use non-standard GNU/LLVM extensions.

## License

Unless otherwise stated, all software found in this repository are
licensed under the MIT License. See the [LICENSE](LICENSE) file for details.
