# sh
This repository is a collection of my scripts. Most of them are written in POSIX Shell, i.e. `sh`. Some are used everyday, while others were written for very specific occasions.

## Installation

```sh
sudo make install
```

## Uninstallation

```sh
sudo make uninstall
```

## Notable scripts
- [mapexec](src/mapexec): Open stdin in `$EDITOR` and execute commands against all, modified, unmodified or wiped lines synchronously or asynchronously. Basically [batch](https://github.com/alexherbo2/batch) but adhering to the Unix philosophy, more versatile and (in my opinion) better.
- [contexec](src/contexec): Continuously execute a file every time it is modified. This is especially useful if you are iteratively processing data using an interpreted or hot-compiled language.
- [selfl](src/selfl): Select a file or directory with `dmenu`.
- [ffmw](src/ffmw): `ffmpeg` wrapper for common actions.
- [tglapp](src/tglapp): Toggle an application on/off based on hashcodes.
- [eln](src/eln): Batch edit or create symlinks.
- [latexstp](src/latexstp): Basically `latexmk` but sucks less and is more minimal.
- [xins](src/xins): Like `xargs` but for standard input.
- [kdialog](src/kdialog): `kdialog` wrapper to not use awful file dialogues.
- [dwmpad](src/dwmpad): Dynamic scratchpads for [dwm](https://dwm.suckless.org). For optimum operation, patch the source: see my complementary dwm [here](https://github.com/XPhyro/dwm-xphyro). dwm is not required for operation, but the script was designed with it in mind.
- [bspwmpad](src/bspwmpad): Dynamic scratchpads for [bspwm](https://github.com/baskerville/bspwm). For optimum operation, append `seq 0 9 | while IFS= read -r i; do bspc rule -a "*:bspwmpad$i" sticky=on state=floating hidden=on; done` to your `bspwmrc`. bspwm is not required for operation, but the script was designed with it in mind.
- [scratchpad](src/scratchpad): Take and manage notes with `vim` and `dmenu`. Can be used with `dwmpad` or `bspwmpad`.
- [notiflog](src/notiflog): Log notifications on the dbus interface `org.freedesktop.Notifications`. Supports locking logging: after initiating the lock, the next notification is not logged. Then it unlocks itself automatically.
- [neomuttpick](src/neomuttpick): Use [kdialog](src/kdialog) to pick attachments in `neomutt`. Add `macro compose A "<shell-escape>neomuttpick<enter><enter-command>source /tmp/neomuttpick<enter>" "Attach With File Manager"` to your `muttrc` file to use.

## Notice that
- Some of these scripts assume that some of the other scripts are in your PATH.
- Some of these scripts (extremely rarely) assume that some of the programs/scripts from [c](https://github.com/XPhyro/c) or [py](https://github.com/XPhyro/py) are in your PATH. Scripts with such assumptions are specialised, such as [popcal](src/popcal) using [calctdiff](https://github.com/XPhyro/py/tree/master/calctdiff) from [py](https://github.com/XPhyro/py) or [sless](src/sless) using [printn](https://github.com/XPhyro/c/tree/master/printn) from [c](https://github.com/XPhyro/c).
- None of the scripts pre-check for the availability of their dependencies.
- In some scripts, GNU-specific options are used (for instance, sed -i).
- All scripts were written with only Linux in mind. Although they will often be compatible with all Unix-like and Unix-derived systems, they might not always be portable to non-Linux systems. Very rarely, Arch Linux is assumed. Scripts assuming Arch Linux can be easily identified by name or explanation.
- For some of the scripts to work correctly, you need my custom build of [dmenu](https://github.com/XPhyro/dmenu-xphyro). This build introduces the -sel, -pc and -snp options.

## Related
- You can find my gpup management scripts [here](https://github.com/XPhyro/gpupmanager).
- You can find the scripts that I use to install an Arch Linux system (to my liking) [here](https://github.com/XPhyro/archinstall).

# License
Unless otherwise stated, all software found in this repository is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.
