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

### [getfl](getfl)
- Usage: `getfl key`
- Description: 

### [getflloc](getflloc)
- Usage: `getflloc key`
- Description: 

### [getloc](getloc)
- Usage: `getloc key`
- Description: 

### [getlocall](getlocall)
- Usage: `getlocall key`
- Description: 

### [getlocunsafe](getlocunsafe)
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

### [m](m) 
- Usage: `m key`
- Description: Marks the current directory with the key `key`.

### [mvloc](mvloc)
- Usage: `mvloc location file1 ... fileN`
- Description: Is equivalent to `mv file1 ... fileN "$(getloc location)"`.

### [setfl](setfl)
- Usage: `setfl`
- Description: Should be run every time after `"$HOME/documents/sys-admin/locations"` is edited.

### [setloc](setloc)
- Usage: `setloc`
- Description: Should be run every time after `"$HOME/documents/sys-admin/files"` is edited.

## Usage

### Interactive shells
PLACEHOLDER

### Scripts
PLACEHOLDER

## Examples
As an example, `getloc wallpaper` returns the path of the directory with the key `wallpaper` in, on average, 4.0 ms. `getfl` is a bit slower, with an average of 6.3 ms. Unsafe counterparts (i.e. `getlocunsafe` and `getflunsafe`) are respectively 1 and 3 ms faster. Apart from the main `getloc` and `getfl` utilities, there are `getflloc` and `getlocall`.

## Other
All benchmarks were done on a laptop with i7-8750H, 2667 MHz CL 15 RAM and a 7200 RPM HDD using `hyperfine` with sufficiently large number of warmup and benchmark iterations and `dash` (version 0.5.11.3) as `sh`.
