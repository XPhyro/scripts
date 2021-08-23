# scripts
This repository is a collection of my scripts. Most of them are written in POSIX Shell, i.e. `sh`, but this is not an sh-specific repository. Some are everyday scripts, whereas others were written for very specific purposes.

## Installation

```sh
sudo make install
```

## Uninstallation

```sh
sudo make uninstall
```

## Environmental Variables
Some scripts use optional or mandatory environmental variables for programs. Assign either `bspwmpad` or `dwmpad` to `$PAD`, `rofi -dmenu` or `dmenu` to `$MENU`, and the respective program will be used. Other used variables include: `$BROWSER`, `$EDITOR`, `$OPENER`, `$PAGER`, `$SHELL`, `$TERMINAL`, `$VPN`, `$_BROWSER`. Some scripts will not respect these as they use program-specific options, such as `rofi -font` or `st -d`.

## Notable scripts
- [bspwmpad](src/sh/bspwmpad): Dynamic scratchpads for [bspwm](https://github.com/baskerville/bspwm). For optimum operation, append `seq 0 9 | while IFS= read -r i; do bspc rule -a "*:bspwmpad$i" sticky=on state=floating hidden=on; done` to your `bspwmrc`. bspwm is not required for operation, but the script was designed with it in mind.
- [contexec](src/sh/contexec): Continuously execute a file every time it is modified. This is especially useful if you are iteratively processing data using an interpreted or hot-compiled language.
- [dwmpad](src/sh/dwmpad): Dynamic scratchpads for [dwm](https://dwm.suckless.org). For optimum operation, patch the source: see my complementary dwm [here](https://github.com/XPhyro/dwm-xphyro). dwm is not required for operation, but the script was designed with it in mind.
- [eln](src/sh/eln): Batch edit or create symlinks.
- [ffmw](src/sh/ffmw): `ffmpeg` wrapper for common actions.
- [kdialog](src/sh/kdialog): `kdialog` wrapper to not use awful file dialogues.
- [latexstp](src/sh/latexstp): Basically `latexmk` but sucks less and is more minimal.
- [mapexec](src/sh/mapexec): Open stdin in `$EDITOR` and execute commands against all, modified, unmodified or wiped lines synchronously or asynchronously. Basically [batch](https://github.com/alexherbo2/batch) but adhering to the Unix philosophy, more versatile and (in my opinion) better.
- [neomuttpick](src/sh/neomuttpick): Use [kdialog](src/sh/kdialog) to pick attachments in `neomutt`. Add `macro compose A "<shell-escape>neomuttpick<enter><enter-command>source /tmp/neomuttpick<enter>" "Attach With File Manager"` to your `muttrc` file to use.
- [notiflog](src/py/notiflog): Log notifications on the dbus interface `org.freedesktop.Notifications`. Supports locking logging: after initiating the lock, the next notification is not logged. Then it unlocks itself automatically.
- [scratchpad](src/sh/scratchpad): Take and manage notes with `vim` and `dmenu`. Can be used with `dwmpad` or `bspwmpad`.
- [selfl](src/sh/selfl): Select a file or directory with `dmenu`.
- [tglapp](src/sh/tglapp): Toggle an application on/off based on hashcodes. Originally written for toggling applications with the same hotkey via `sxhkd`.
- [xins](src/sh/xins): Like `xargs` but for standard input.

## Notice that
- Some of these scripts assume that some of the other scripts are in your `PATH`.
- None of the scripts pre-check for the availability of their dependencies.
- In some scripts, GNU-specific options are used (for instance, `sed -i`).
- All scripts were written with only Linux in mind. Although they will often be compatible with all Unix-like and Unix-derived systems, they might not always be portable to non-Linux systems.
- For some of the scripts to work correctly, you need my custom build of [dmenu](https://github.com/XPhyro/dmenu-xphyro). This build introduces the -sel, -pc and -snp options.

## Related
- You can find my gpup management scripts [here](https://github.com/XPhyro/gpupmanager).
- You can find the scripts I use to install an Arch Linux system (to my liking) [here](https://github.com/XPhyro/archinstall).

# License
Unless otherwise stated, all software found in this repository is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.
