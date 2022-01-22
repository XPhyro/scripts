[![CodeFactor](https://www.codefactor.io/repository/github/xphyro/scripts/badge)](https://www.codefactor.io/repository/github/xphyro/scripts)
[![Language grade: Python](https://img.shields.io/lgtm/grade/python/g/XPhyro/image-entropy.svg?logo=lgtm&logoWidth=18)](https://lgtm.com/projects/g/XPhyro/image-entropy/context:python)

# scripts
This repository is a collection of my scripts. Most of them are written in POSIX
Shell, i.e. `sh`, but this is not an sh-specific repository.

Some are general utility scripts, whereas others are written for very specific
purposes.

## Installation

To install for your user:

    make install

To install system-wide:

    sudo make install

You may change the installation directory by setting `$PREFIX`.

If you would like to use the provided wrapper scripts, add `$PREFIX/bin/wrapper`
to your `$PATH` with higher priority than the locations of the wrapped scripts.

## Uninstallation

If you installed for your user:

    make uninstall

If you installed system-wide:

    sudo make uninstall

## Notable Scripts
- [afgrep](src/c/util/core/afgrep.c): Like `grep -F`, but supports alignment and
  offset. See `afgrep -h`.
- [bspwmpad](src/sh/bspwm/util/bspwmpad): Dynamic scratchpads for
  [bspwm](https://github.com/baskerville/bspwm). For optimum operation, include
  `seq 0 9 | while IFS= read -r i; do bspc rule -a "*:bspwmpad$i" state=floating
  hidden=true; done` in your `bspwmrc`. bspwm is not required for operation, but
  the script was designed with it in mind. See `bspwmpad -h`. There is also
  [dwmpad](src/sh/.archived/dwm/util/dwmpad).
- [clplog](src/sh/daemon/clplog): Log clipboard history.
- [clplogynk](src/sh/hotkey/clplogynk): Use `rofi` to yank an element from the
  clipboard history.
- [cmath](src/c/util/math/cmath.c): A NumPy-like interface for the `math.h` C
  standard header, enabling fast mathematical computations of an array of
  numbers in the shell. See `cmath -h`.
- [contexec](src/sh/daemon/contexec): Continuously execute a file every time it
  is modified. This is especially useful if you are iteratively processing data
  using an interpreted or hot-compiled language. See `contexec -h`.
- [eln](src/sh/ishell/eln): Batch edit or create symlinks.
- [expandpath](src/c/util/core/expandpath.c): Like `wordexp`, but only expands `~`.
  See `expandpath -h`.
- [ffmw](src/sh/softwrapper/ffmw): `ffmpeg` wrapper for common actions. See
  `ffmw -h`.
- [fmaps](src/c/util/core/fmaps.c): Map stdin per given key-value pairs. See `fmaps
  -h`.
- [kdialog](src/sh/wrapper/kdialog): `kdialog` wrapper to not use awful file
  dialogues.
- [latexstp](src/sh/daemon/latexstp): Basically `latexmk` but sucks less and is
  more minimal. See `latexstp -h`.
- [mapexec](src/sh/util/mapexec): Open stdin in `$EDITOR` and execute commands
  against all, modified, unmodified or wiped lines synchronously or
  asynchronously. Basically [batch](https://github.com/alexherbo2/batch), but
  more versatile. See `mapexec -h`.
- [mkparent](src/c/util/sys/mkparent.c): Create the parent directories of a path.
- [neomuttpick](src/sh/integration/neomutt/neomuttpick): Use
  [kdialog](src/sh/wrapper/kdialog) to pick attachments in `neomutt`. Add `macro
  compose A "<shell-escape>neomuttpick<enter><enter-command>source
  /tmp/neomuttpick<enter>" "Attach With File Manager"` to your `muttrc` file to
  use.
- [notiflog](src/py/daemon/notiflog): Log notifications on the dbus interface
  `org.freedesktop.Notifications`. Supports locking logging: after initiating
  the lock, the next notification is not logged. Then it unlocks itself
  automatically. Supports blacklisting: certain notifications can be blacklisted
  and logged to a different file.
- [scratchpad](src/sh/hotkey/scratchpad): Take and manage notes with `vim` and
  `dmenu`. Can be used with `dwmpad` or `bspwmpad`.
- [selfl](src/sh/util/selfl): Select a file or directory with `$MENU`.
- [stest](src/c/util/core/stest.c): Filter a list of files by properties. This is
  different from the `stest` included with suckless `dmenu`. See `stest -h`.
- [sumsize](src/py/util/sumsize): Sum human readable sizes. See `sumsize -h`.
- [tglapp](src/sh/hotkey/util/tglapp): Toggle an application on/off based on
  hashcodes. Originally written for toggling applications with the same hotkey
  via `sxhkd`. See `tglapp -h`.
- [unexpandpath](src/c/util/core/unexpandpath.c): Undo `expandpath`. See
  `unexpandpath -h`.
- [wordexp](src/c/util/core/wordexp.c): Perform word expansion like a POSIX shell.
  See `man 3 wordexp`.
- [xins](src/sh/util/xins): Like `xargs` but for standard input. See `xins -h`.

## Environment Variables
Some scripts use optional or mandatory environment variables for applications.
Assign either `bspwmpad` or `dwmpad` to `$PAD`, `rofi -dmenu` or `dmenu` to
`$MENU`, and the respective application will be used. Other used variables
include: `$BROWSER`, `$EDITOR`, `$OPENER`, `$PAGER`, `$SHELL`, `$TERMINAL`,
`$VPN`, `$_BROWSER`. Some scripts will not respect these as they use
application-specific options, such as `rofi -font` or `st -d`.

In all shell scripts, if you set `$SHELL_VERBOSE` to greater than 0, `set -x` is
executed and all executed commands are printed to stderr.

## Notice
- Some of these scripts assume that some of the other scripts are in your
  `$PATH`.
- None of the scripts pre-check for the availability of their dependencies.
- In some scripts, GNU extensions are used (for instance, `sed -i` or
  `strcasestr`).
- All scripts were written with only Linux in mind. Although they will often be
  compatible with all Unix-like and Unix-derived systems, they might not always
  be portable to non-Linux systems.

## Related
- You can find my gpup management scripts
  [here](https://github.com/XPhyro/gpupmanager).
- You can find the scripts I use to install an Arch Linux system (to my liking)
  [here](https://github.com/XPhyro/archinstall).

# License
Unless otherwise stated, all software found in this repository are
licensed under the MIT License. See the [LICENSE](LICENSE) file for details.
