[![GitHub](https://img.shields.io/badge/GitHub-2B3137?style=flat-square&logo=GitHub&logoColor=FFFFFF)](https://github.com/XPhyro/scripts)
[![Codeberg](https://img.shields.io/badge/Codeberg-2185D0?style=flat-square&logo=codeberg&logoColor=F2F8FC)](https://codeberg.org/XPhyro/scripts)
[![CodeFactor](https://www.codefactor.io/repository/github/xphyro/scripts/badge?style=flat-square)](https://www.codefactor.io/repository/github/xphyro/scripts)
[![GitHub Build Workflow Status](https://img.shields.io/github/actions/workflow/status/XPhyro/scripts/build.yml?branch=master&style=flat-square&label=build%20%26%20test)](https://github.com/XPhyro/scripts/actions/workflows/build.yml)
[![GitHub Analyse Workflow Status](https://img.shields.io/github/actions/workflow/status/XPhyro/scripts/analyse.yml?branch=master&style=flat-square&label=analyse)](https://github.com/XPhyro/scripts/actions/workflows/analyse.yml)
[![GitHub Spell Workflow Status](https://img.shields.io/github/actions/workflow/status/XPhyro/scripts/spell.yml?branch=master&style=flat-square&label=spell)](https://github.com/XPhyro/scripts/actions/workflows/spell.yml)

# scripts

This repository holds a collection of over 500 scripts, utilities and libraries
for Unix-like operating systems, OBS plugins and browser extensions, written in:
Awk, Bash, C , C++, execline, Go, JavaScript, POSIX Shell, Perl, Python, and Rust.

## Installation

```sh
git clone https://github.com/XPhyro/scripts.git # clone repository
git submodule update --init --recursive # initialise dependencies

make install # install for your user
sudo make install # or install system-wide (recommended)
```

You may set `$PREFIX` to change the installation directory. By default, it is
`~/.local` for your user, or `/usr/local` for the system.

After installation, add `$PREFIX/share/scripts/include` to your path. A POSIX
shell standard library will have been installed here, and will be used by all
shell scripts.

If you would like to use the provided wrapper scripts, also add `$PREFIX/bin/wrapper`
to your `$PATH` with higher priority than the locations of the wrapped scripts.

## Uninstallation

```sh
make uninstall # if you installed for your user
sudo make uninstall # if you installed system-wide
```

You do not need to re-set `$PREFIX` while uninstalling. The previously-used
`$PREFIX` will automatically be used.

If you altered your `$PATH`, you may also wish to undo it.

## Notable Scripts

| Name                      | Description                                                                                                     |
| :------------------------ | :-------------------------------------------------------------------------------------------------------------- |
| agpg                      | Encrypt/decrypt files using `gpg`.                                                                              |
| argn                      | Slice arguments or standard input using Python's slice syntax.                                                  |
| backuppath                | Generate new paths for files compliant with GNU's `--backup`.                                                   |
| bitspersec                | Measure output speed from standard input.                                                                       |
| blank-screens             | Fade out/in individual monitors without turning them off.                                                       |
| calctdiff                 | Calculate time between dates.                                                                                   |
| cast                      | Cast between various types.                                                                                     |
| chomp                     | Remove last newline of standard input.                                                                          |
| contexec                  | Continuously execute files on change.                                                                           |
| dev2sh                    | Convert `evdev`-compatible devices' inputs to virtual keyboard & mouse macros, or dispatch shell commands.      |
| exif-filter               | Filter files by EXIF properties.                                                                                |
| expandpath & unexpandpath | Perform path expansion similar to shells, or undo path expansions.                                              |
| ffmw                      | Use `ffmpeg` with natural command names.                                                                        |
| gcc-otg                   | Just-in-time compile and run C & C++ source files like Python.                                                  |
| gi                        | Terminal UI for Git.                                                                                            |
| headsetcontrol2mpris      | Convert `headsetcontrol`-compatible headset events into MPRIS commands.                                         |
| isdaytime                 | Check if its day or night time.                                                                                 |
| l                         | Conditionally page or preview files.                                                                            |
| latexd                    | Automatic continuous compilation for LaTeX.                                                                     |
| lck                       | Mutexes for the shell.                                                                                          |
| lf-run                    | File & image previewing for `lf`, with caching support. See also `lf-thumbnail`, `lf-preview` and `lf-cleaner`. |
| maptouch                  | Map touch screens to X11 screens in environments that don't properly do this.                                   |
| memory-usage              | Print memory usage sorted by application.                                                                       |
| mvbulk                    | Rename, move or delete files in bulk using your preferred editor.                                               |
| neomutt-pick              | Use a file picker with `neomutt`.                                                                               |
| nsts                      | Easily run NIST cryptographic tests.                                                                            |
| numsh                     | Like NumPy, but for the shell.                                                                                  |
| openrgb-cycle             | Manage OpenRGB profiles.                                                                                        |
| pathfinder                | Mark and jump to directories & files in the terminal.                                                           |
| personality               | Calculate Myers-Briggs personality type distributions.                                                                      |
| plot-git-log              | Plot a pretty graph of Git history.                                                                             |
| prefix                    | Add various prefixes to standard input and pass to standard output.                                             |
| presentimg                | Easily tile images with optional borders or labels.                                                             |
| previewimage              | Preview images in the terminal.                                                                                 |
| previewtext               | Preview files with useful meta-information or syntax highlighting.                                              |
| reparent-window           | Reparent X11/Xwayland windows onto other X11/Xwayland windows.                                                  |
| scrolls                   | Simulate fixed-canvas text scroll in the terminal. Useful for status bars.                                      |
| shell-escape              | Sanitize strings for shells.                                                                                    |
| shufr                     | Like `shuf`, but supports consecutive uniqueness.                                                               |
| slash                     | Normalise slashes of paths.                                                                                     |
| sleep-precise             | Like `sleep`, but with millisecond precision and ~3ms accuracy.                                                 |
| span-images               | Merge images into a single image using X11 geometry syntax.                                                     |
| std::string               | C++'s `std::string` for the shell.                                                                              |
| std::unordered_map        | C++'s `std::unordered_map` for the shell.                                                                       |
| std::vector               | C++'s `std::vector` for the shell.                                                                              |
| stest                     | Test files for various conditions like `dmenu`'s `stest`, but with more tests & support for partial inversions. |
| sumduration               | Sum human-readable durations.                                                                                   |
| sumsize                   | Sum human-readable sizes.                                                                                       |
| sxhkd-auto                | Wrapper around sxhkd to use different configurations depending on the current environment.                      |
| tabutils                  | Copy/paste tabs in Chromium/Firefox-based browsers.                                                             |
| termctl                   | Control the terminal with natural command names.                                                                |
| tglapp                    | Toggle applications. Useful for hotkeys.                                                                        |
| torso                     | Like `head` & `tail`, but prints the in-between.                                                                |
| trimpdf                   | Trim excess whitespace from PDFs.                                                                               |
| unsymlinks                | Reverse the effect of `symlinks`.                                                                               |
| usbctl                    | Control USB devices.                                                                                            |
| volmicmute                | Toggle mute status of the default microphone.                                                                   |
| wallpaper                 | Set periodically-changing image or video wallpapers.                                                            |
| warnbattery               | Issue audible warnings for different battery charge thresholds.                                                 |
| x13ctl                    | Control ASUS ROG Flow X13 and other ASUS ROG laptops.                                                           |
| ytdl-or-gldl              | Use `yt-dlp`, `youtube-dl` or `gallery-dl` depending on availability and target web site support.               |
| zraminit                  | Initialise zram.                                                                                                |

There are many other scripts & programs apart from these. Take a look under
[src/](src/).

Apart from executables, there are also C & C++ libraries, and even a [home brew
standard library](src/sh/include/) for POSIX shells!

## Usage

See `man COMMAND`, `COMMAND -h` or `COMMAND --help`. Trivial programs & scripts
do not have man pages or help dialogues, either infer usage from their names,
or see the source code.

In all shell scripts, if you set `$SHELL_VERBOSE` to greater than 0, `set -x`
is executed and all executed commands are printed to standard error.

## Caveats

- Some scripts assume that some of the other scripts are in your `$PATH`, so
  partial installation is not recommended.
- In some scripts, GNU extensions like `sed -i` or `strcasestr` are used, so
  they may not work on strictly-POSIX environments.
- Some scripts were written with only Linux in mind, although they may be
  compatible with other Unixes and Unix-likes, or may be ported with minimal
  modification.
- All scripts/programs are only tested on Linux.
- C and C++ code are only tested with the latest release of GCC. Most programs
  are compatible with any POSIX/ISO-conforming compilers but some use
  non-standard GNU/LLVM extensions.

## Contributing

Please see [CONTRIBUTING.md](CONTRIBUTING.md).

## License

Unless otherwise stated, all software found in this repository are
licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

## Star History

<a href="https://star-history.com/#XPhyro/scripts&Date">
  <picture>
    <source media="(prefers-color-scheme: dark)" srcset="https://api.star-history.com/svg?repos=XPhyro/scripts&type=Date&theme=dark" />
    <source media="(prefers-color-scheme: light)" srcset="https://api.star-history.com/svg?repos=XPhyro/scripts&type=Date" />
    <img alt="Star History Chart" src="https://api.star-history.com/svg?repos=XPhyro/scripts&type=Date" />
  </picture>
</a>
