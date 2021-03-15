# sh
This repository is a collection of my shell scripts. Some are used everyday, while others were written for very specific purposes that I do not carry now. Some of the scripts are not included, as they have their own repositories. Be also aware that some of the scripts might assume that some of the other scripts are in your PATH.

## Requisites
- Some of these scripts assume that some of the other scripts are in your path.
- For some of the scripts to work correctly, you need my custom build of [dmenu](https://github.com/XPhyro/dmenu-xphyro). This build introduces the -sel, -pc and -snp options.

## Notable Scripts
- *mapexec*: Open stdin in `$EDITOR` and execute commands against all, modified, unmodified or wiped lines. Basically [batch](https://github.com/alexherbo2/batch) but (in my opinion) better.
- *selfl*: Select a file or directory with dmenu.
- *ffmw*: ffmpeg wrapper for common actions I use.
- *tglapp*: Toggle an application on/off based on hashcodes.
- *eln*: Batch edit or create symlinks.
- *latexstp*: Basically latexmk but sucks less and is less bloated.

## Related
- You can find my gpup management scripts [here](https://github.com/XPhyro/gpupmanager).
- You can find the scripts that I use to install an Arch Linux system (to my liking) [here](https://github.com/XPhyro/archinstall).
