# Pathfinding suite
A pathfinding suite designed to save time in an interactive shell and set standards between scripts.

## Overview
The suite delivers the following features:
- Getting the full paths of special directories and files by a key extremely quickly.
- Navigating between special directories or the directories of special files.
- Moving files to special directories.
- Editing, printing or viewing special files, whatever your current working directory is.
- Marking your current working directory with a key, allowing similar but slower functionality as in pre-set directories. A special mark, `¬`, is exported every time you change your current working directory.

The suite will deliver the following features:
- Marking files.

Explanation of each command:
- *[_@](_@)*:
- *[_@i](_@i)*:
- *[_fd](_fd)*:
- *[_ff](_ff)*:
- *[_g](_g)*:
- *[_gi](_gi)*:
- *[_¬](_¬)*:
- *[batfl](batfl)*:
- *[catfl](catfl)*:
- *[cm](cm)*:
- *[getfl](getfl)*:
- *[getflloc](getflloc)*:
- *[getloc](getloc)*:
- *[getlocall](getlocall)*:
- *[getlocunsafe](getlocunsafe)*:
- *[m](m)*:
- *[mvloc](mvloc)*:
- *[setfl](setfl)*:
- *[setloc](setloc)*:

## Usage

### Interactive shells
PLACEHOLDER

### Scripts
PLACEHOLDER

## Examples
As an example, `getloc wallpaper` returns the path of the directory with the key `wallpaper` in, on average, 4.0 ms. `getfl` is a bit slower, with an average of 6.3 ms. Unsafe counterparts (i.e. `getlocunsafe` and `getflunsafe`) are respectively 1 and 3 ms faster. Apart from the main `getloc` and `getfl` utilities, there are `getflloc` and `getlocall`.

## Other
All benchmarks were done on a laptop with i7-8750H, 2667 MHz CL 15 RAM and a 7200 RPM HDD using `hyperfine` with sufficiently large number of warmup and benchmark iterations and `dash` (version 0.5.11.3) as `sh`.
