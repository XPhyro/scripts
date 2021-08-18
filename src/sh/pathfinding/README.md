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

## Installation

1. Follow the installation instructions in [../README.md](../README.md).
2. Source [.shrc](.shrc) in your shell rc file or append it in-place.

## Explanation of commands (to be completed)

### [_@](_@)
- Usage: `@ key`
- Description: 

### [_@i](_@i)
- Usage: `@i`
- Description: 

### [_fd](_fd)
- Usage: `fd`
- Description: 

### [_ff](_ff)
- Usage: `ff`
- Description: 

### [_g](_g)
- Usage: `g key`
- Description: 

### [_gi](_gi)
- Usage: `gi`
- Description: 

### [_¬](_¬)
- Usage: `¬`
- Description: 

### [batfl](batfl)
- Usage: `batfl key`
- Description: 

### [catfl](catfl)
- Usage: `catfl key`
- Description: 

### [cm](cm)
- Usage: NA
- Description: This is a utility script used by the other scripts. It is not meant to be used interactively.

### [e](e)
- Usage: `e key`
- Description: 

### [ei](ei)
- Usage: `ei`
- Description: 

### [getfl](getfl)
- Usage: `getfl key`
- Description: 

### [getflloc](getflloc)
- Usage: `getflloc key`
- Description: 

### [getflunsafe](getflunsafe)
- Usage: `getflunsafe key`
- Description: 

### [getdir](getdir)
- Usage: `getdir key`
- Description: 

### [getdirall](getdirall)
- Usage: `getdirall key`
- Description: 

### [getdirunsafe](getdirunsafe)
- Usage: `getunlocsafe key`
- Description: 

### [getrealfl](getrealfl)
- Usage: `getrealfl key`
- Description: 

### [getrealflloc](getrealflloc)
- Usage: `getrealflloc key`
- Description: 

### [getrealloc](getrealloc)
- Usage: `getrealloc key`
- Description: 

### [getreallocall](getreallocall)
- Usage: `getreallocall key`
- Description: 

### [getreallocunsafe](getreallocunsafe)
- Usage: `getreallocunsafe key`
- Description: 

### [i](i) 
- Usage: `i [key executable file]...`
- Description: Dynamic merge of [e](e), [open](open) and [v](v).

### [m](m) 
- Usage: `m key`
- Description: Marks the current directory with the key `key`.

### [mvloc](mvloc)
- Usage: `mvloc location file1 ... fileN`
- Description: Is equivalent to `mv file1 ... fileN "$(getdir location)"`.

### [open](open)
- Usage: `open executable`
- Description: 

### [setall](setall)
- Usage: `setall`
- Description: Alias for running `setfl` and `setdir`.

### [setfl](setfl)
- Usage: `setfl`
- Description: Should be run every time after `"$HOME/documents/sys-admin/locations"` is edited.

### [setdir](setdir)
- Usage: `setdir`
- Description: Should be run every time after `"$HOME/documents/sys-admin/files"` is edited.

### [v](v)
- Usage: `v ARGS...`
- Description: Alias for running `"$EDITOR" "$@"` where positional arguments that are files inside the directory `src` are replaced with `src/ARG`.

## Usage

### Interactive shells
PLACEHOLDER

### Scripts
PLACEHOLDER

## Examples
As an example, `getdir wallpaper` returns the path of the directory with the key `wallpaper` in, on average, 4.0 ms. `getfl` is a bit slower, with an average of 6.3 ms. Unsafe counterparts (i.e. `getdirunsafe` and `getflunsafe`) are respectively 1 and 3 ms faster. Apart from the main `getdir` and `getfl` utilities, there are `getflloc` and `getdirall`.

## Other
All benchmarks were done on a laptop with i7-8750H, 2667 MHz CL 15 RAM and a 7200 RPM HDD using `hyperfine` with sufficiently large number of warmup and benchmark iterations and `dash` (version 0.5.11.3) as `sh`.
