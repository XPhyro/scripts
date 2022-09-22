# High Priority

## Security
- `eval`ing `getpath`, `xdotool` and other programs can lead to security
  problems if the scripts are run with elevated permissions. Warn the user and,
  for programs local to this repository, always install the `eval`ed programs as
  root to prevent tinkering.
- Do not `eval` the output of `headsetcontrol` in `polybar-headsetcontrol` in
  case the interface changes.
  - Maybe send a PR for an `-e` (for eval) flag to `headsetcontrol`?

## Bugs
- `acpihandler` does not handle already-off keyboard/touchpad correctly on PROG1
  event.

## Features
- Add the ability to have default arguments via environment variables in
  `tglapp`.
- In `bspwmpad`, if a slot is requested and the requested slot is occupied,
  check if the occupier is alive. If a slot is not requested and all slots are
  occupied, check all of the slots for whether each occupier is alive.
- Do not hardcode paths like `/home/xphyro` in unit tests.
- In `bspwmpad`, add an option to start in some desktop.
- Write a script that hard-wraps `make` and implements a `git`-like workflow. In
  other words, `make` should try `make`ing parents until it finds a `Makefile`.
- In `bspwmpad`, support non-automated non-incrementing keycodes in addition to
  numbers.
- Support arbitrary counts of warnings and configurations in `warnbattery`.
- In `tglapp`, support terminating the X window instead of the run process.
- Support custom lock directory prefix in `dbutil.h`.
- Support custom lock directory prefix in `lck`.
- Support animated wallpapers via `mpv` in `wallpaper`.
- Support matching aspect ratios and displays in `wallpaper`.
- Implement for/while-else construct of Python in C++.
  [This](https://stackoverflow.com/a/25209781/8403070) could be used as a
  starting point.
- Add an option to override or append to the default clean-up process on
  toggling off in `tglapp`.
  - This would be useful for double-forking programs and or those that have
    weird process trees like Steam.
- Make the `kdialog` wrapper cross-compatible with other dialogue applications
  such as Zenity. Create wrappers for these newly supported applications that
  `exec` the `kdialog` wrapper.
  - Or, create a generic base file dialogue application that uses `lf` and
    create wrappers for other file dialogue applications that `exec` this
    generic base application.
- Currently, if `fillline` is included in an on-terminal-initialisation
  pipeline, the line might not be filled fully. This is not per se a bug on the
  `fillline` end, but it would be nice to mitigate it. Add an option to wait a
  tiny bit to allow for the terminal window to initialise and settle to its
  permanent/semi-permanent row/column configuration. Add another option to wait
  for the terminal row/column values to change. Add another option to limit the
  waiting time of the previous action, so it does not end up waiting
  indefinitely.

## New Scripts
- Write a daemon that enables having different keyboard layouts for each X
  client.
- Finish `registernotif`.
- `loginfo`, `logwarn` and `logerr`; similar to `registernotif`.
- `logTYPEnotif`: wrapper for `logTYPE "$@"; registernotif "$@"`.
- Gesture-like daemon for built-in mixer states via `headsetcontrol`.
- Write an `xsetwacom` wrapper. See
  [this](https://wiki.archlinux.org/title/wacom_tablet#Configuration).
- Write a C program that takes in argv through argv and parsing configuration
  through stdin and outputs shell `eval`able code, then replace all applicable
  existing argument parsing in shell scripts with this program.
- Same as `warnbattery`, except for `headsetcontrol`.
- Create a script for enabling/disabling/toggling/querying xinput devices. Use
  this script in `acpihandler`, `maptouch`, `tpcycle`, and other applicable
  scripts.
- Automate [PulseAudio/PipeWire-Pulse network
  streaming](https://gist.github.com/savegame/58ae5966c58a71fda5d3800b335eb2f5)
  with optional pair configuration over `ssh`.
- Merge `*maps` into a single C/C++ program.

## Refactoring / Rewriting / Reworking
- Optimise `dbutil.h` to do less allocations.
  - [ ] Reuse built paths.
- Rewrite `sumsize` in C/C++.
- Rewrite `cast` with size-aware conversion.

## Other
- Documentation:
  - Online documentation via `github.io` & `codeberg.page`.
  - Offline documentation via man pages.
  - `-h` and/or `--help` flags.
  - For interpreted stuff and config files, brief explanation as comment at the
    beginning of the file.
  - Install man pages as compressed with `gzip`.
- Add `cppcheck` to analyse target in `.make`.
- Maybe install C/C++ headers?
- Parse the beginning of C/C++ programs to set custom compiler/linker flags.
- Do not clean C/C++ objects by default and use them to not re-compile unchanged
  translation units.


# Normal Priority

## Bugs
- `ffmw decat` does not work as expected. When the time is in format `HH:MM:SS`,
  it seems to work. Maybe it requires this format? If so, add it in the help
  menu.
- Rewrite `parseargs()` without `while [ -n "$1" ]` to allow for empty
  arguments. Instead loop using the count of arguments or use `for i`.
- In `tglapp`, `--list=compact` prints stdout and stderr.
- In `bspwm-flwall`, do not follow if the node added or removed is not in the
  current desktop, even if it's in another monitor's focused desktop.
- `clplog` sometimes does not release the lock, fix.
- `wallpaper -x xwallpaper` assumes `-n 2` and that there are two output
  displays.

## Features
- In `contexec`, show the output in a `$PAD` and open the editor in the
  initialised terminal.
- Allow `m` to take input files. If an input file is given, mark the file; if
  not, mark the current working directory. Similarly, support those files in `@`
  for editing.
- In `contexec`, show the execution count in the header.
- In `bspwmpad`, allow setting the font size (not the whole font). This would be
  useful in scripts/programs like `weather`, `neomutt`, etc.
- Add options in `latexstp` to not open the source/output files.
- Add an option in `contexec` to open the editor. When this option is passed,
  open the editor in the current terminal and execute the file in a `$PAD`.
- Add an option in `bspwmpad` that would skip the acquisition of a lock if all
  locks are occupied. This could have two behaviours: open up a normal st
  session or execute the command without st. This could be made into two more
  options that would determine the behaviour.
- When setting a mark, ensure that the mark name does not contain %20 (i.e.
  space). Tabs, etc. are ok with the current setup. Alternatively, refactor the
  current setup to not use whitespace delimiters. A mechanism like in `getdir`
  and `getfl` could be implemented.
- Add support for regular files in mark scripts. A possible way could be to also
  add support for passed arguments for files in the marking process and check
  the file type; if it is a regular file, then the file could be marked in a
  different mark set. While using marks, first check if a directory mark exists.
  If there exists a directory mark, `cd` into it; else, open up the regular file
  for the given mark in `$EDITOR` if there is such a mark.
- In `steammgr`, keep a database of application IDs scraped from
  [steamdb](https://steamdb.info/apps) and present the user with relevant
  options. This could be done in two ways: either show the relevant options by
  being command-aware, or first show a meta prompt for all, installed and
  in-library games, and then show the IDs (or show the meta options in the
  installed prompt, and show another prompt with the selected if all or
  in-library is selected).
- Integrate `fzfp` of `stpv` into `f` and `_fd`.
- Integrate *synctex* in `latexstp`. See
  [this](https://www.math.cmu.edu/~gautam/sj/blog/20140310-zathura-fsearch.html)
  and [this](https://gist.github.com/vext01/16df5bd48019d451e078).
- Implement a no-quoting option in `tglapp` like the inverse of the one that
  exists in `mapexec`.
- `ffmpeg` fails to parse the filename (why does it even parse it?) if it
  contains one of the following characters `:,`. Make `ffmw` create a symlink
  pointing to the input file(s) to mitigate the issue.
- In `tglapp`, add support for different commands depending on whether the
  application is open or not. This would be useful in many scenarios, including
  toggling an application wrapped with `screen`.
- Add an option to start applications in a `screen` named `"$keycode"` in
  `tglapp`. Also add an option to overwrite the `screen` name.
- Support `--long-option=VALUE` in `ffmw` and `selfl`.
- Support `-l LIMIT, --limit LIMIT` global option in `ffmw` to limit the CPU
  usage of `ffmpeg` using `cpulimit`.
- In `tglapp`, add support for force restarting applications via only a keycode.
  To do this, read the command to start from `*/cmd`.
- In `tglapp`, add `by-cmd`, `by-kc` (by-keycode), `by-hash` and `by-pid`
  sub-directories like `/dev/*/by-*`.
- Add usage statistics to `g`, `e` and `i` to rename keycodes to more efficient
  ones. Be sure to differentiate between scripted calls and interactive calls,
  this is probably a non-issue as the keycodes would be vastly different.
- Use `get*unsafe` in `_[ge]*`.
- Make the mark '¬' not shared between terminals, but still saved after the
  terminal is closed. Load the value only at initialisation. In the interactive
  shell side, before exiting, `cp -f "$somedir/$$" "$somedir/0"` and when
  initialising, `cp "$somedir/0" "$somedir/$$"` can respectively be used to save
  and load.
- In `tglapp`, implement `-F, --fork` using `setsid --fork -- "$realshell" -c`.
- In `tglapp`, if both `-k` and `$cmd` are given, and if an application with the
  same keycode is currently active, check that the given command is the same as
  the currently running application. If the commands do not check out, exit with
  error and do not toggle any applications.
- In `bspwmpad`, add an option to use `tglapp`.
- In `tglapp`, add an option to use `bspwmpad`. If this option is given, when
  toggling the application, toggle the hidden status of the pad if it is on.
- In `tglapp` add an option to auto-restart the application when it exits. Make
  this behaviour togglable between only non-zero codes and all codes.
- In `bspwm-show`, show thumbnails. To do this, use `rofi` instead of
  `execmenu`.
- Do not assume a font size in `rofifit`. The new calculation should still be
  generous such that it would not make the same mistake as `rofi` trimming
  *very* short.
- In `bspwmpad`, add an option to hide the cursor.
- In `tglapp`, add an option that disables auto-unlocking, allowing the user to
  review the stdout/stderr of the command.
- In `syncmail`, allow adding temporary and permanent blacklist. If a lock is
  present, do not send a notification for the blacklisted senders/titles/bodies.
- In `gcm`, if there is no staged files and if 2 arguments are given, assume the
  second argument is a file.
- In `latexstp`, allow passing arguments to `bspwmpad`.
- In `bspwmpad`, support killing the command running in the given pad number.
- In `bspwmpad`, support killing the command running in the given pad number and
  replacing it. This function should require a valid and non-zero `-n` to be
  passed.
- In C `get*`, add diagnostic error messages.
- In `fmaps`, implement options similar to that of `afgrep`'s `-aexbi`.
  - **Maybe** make these options specific to each given mapping? If so, also
    have the ability to globally define these options.
- Maybe merge `_@` and `_g`?
- In `polybar-headsetcontrol`, keep track of the last known battery percentage.
  If battery is charging, show that it is charging, but also estimate a battery
  percentage.
- In `polybar-headsetcontrol`, make the battery icon's fill follow the
  percentage.
- In `stdutil.h`, min/max/minmax clamped versions add `astrto*` functions.
- Add `-c` option to select column in `sumsize`.

## New Scripts
- Create software-level alternatives to the `bright*` scripts.
- Finish `todo`.
- Write a variant of `volappch` that toggles mute status.
- Migrate `dotfilesbak{,-sensitive}` into this repository, and simply symlink
  them to the original locations.
- Write `getfls` and `getdirs` that support multiple arguments unlike `getfl`
  and `getdir`. This might also be integrated in `getfl` and `getdir` in a
  lightweight manner.
- Write scripts that would: print the value for a mark, print the mark for a
  given value (if marks are always in expanded form, also expand the given
  value; else, ensure that the equivalent values are considered).
- Write a helper script that would make it easy for a script to implement
  hashmaps. Currently `getdir`, `getfl` and other scripts use directory/file
  structures as a workaround. This script would need to perform at least as well
  as the workaround, if not better, and be convenient to use.
- Write a library-ish thing to make it easy for scripts, especially those in
  terminal file managers, to implement short-term smart thrash functionality
  that is considerate of the disk usage of the thrash.
- Timer, chronometer, alarm.
- Implement a crop subcommand in `ffmw`.
- Write a version of `v` (from .zshrc) that has keys as in the pathfinding suite
  depending on the directory. Say, one executes `vm t dotfilesbak-tick` in
  `"$HOME/.dotfiles"`. When they execute `"$scriptname" t`, `dotfilesbak-tick`
  is opened with `"$EDITOR"`. Alternatively, this behaviour could be integrated
  in the currently existing mark functionality either by prioritising
  directory-specific marks or by making the user prepend the key by a character,
  say \_.
- Write a script similar to `bspwmpad` that would register/deregister the
  focused node if the key (1-9) is empty, else it will deregister that key;
  then, write a script that would hide/show these nodes per key. Key 0 should be
  similar to the key 0 of `bspwmpad`.
- Write a script to check an sxhkdrc file (by default
  `"$HOME/.config/sxhkd/sxhkdrc"`) that would check for duplicate hotkeys, *not*
  assuming the modifiers are in a particular order, and print the hotkeys along
  with their commands.
- Using `mapexec`, write a batch renaming tool that passes the name through
  `stat --printf=` if the line starts with ``.
- Write a `bspwm` & `polybar` script that notifies `polybar` to update master
  status.
- Write a manager for *suckless*-like software that handles updating to
  upstream, applying patches to the fresh copy and building & installing. (or
  just update master and execute `git rebase --rebase-merges --force-rebase
  master`)
- `fillcol`: Like `fillline` and `fillterm`, but for columns. Take a column
  number as input. Do not scroll lines, rather overwrite the screen.
- `volauxdefset`
- `volauxsetarr`: like `volauxset`, but moves in predefined volume levels (given
  via arguments).
- `volauxdefsetarr`: like `volauxdefset`, but moves in predefined volume levels
  (given via arguments).
- Write a portable version of `rand` using `arc4random_buf`.
- Write a wrapper script for `xargs` and `xins`: If arguments fit into a single
  invocation, use `xargs`; else, use `xins`.

## Refactoring / Rewriting / Reworking
- Optimise `kmcycle` and `setxkb`.
- Use `xsel` instead of `xclip`.
- In `dotfilesbak{,-sensitive}`, change the author and/or committer of the
  commits that are made by these scripts.
- Optimise `bspwmpadinit`'s PID updating with `inotifywait`.
- Rewrite `brightmute` in a more robust fashion that would not often break.
- Consider `scrot` in `cpscr`.
- Make `dupe`, `getnewpath` and alike use GNU `--backup` syntax instead of
  continuous trailing underscores.
- Make `-fin` options in `ffmw` global as they are used by every non-help
  subcommand.
- Make `mvloc` have the same syntax as `mv`. That is, `mv file1 file2 ... fileN
  location`. `eval "$#=\"$(getdir $$#)\""` could be used to do so.
- Rewrite `getnewname` and `getnewpath` to be compatible with GNU's
  `--backup=numbered`.
- Rewrite `eln` using `mapexec`. This will allow the name to be arbitrary
  (except containing a newline).
- Rewrite `kasel` using `pgrep` and `kill --timeout`.
- Use `inotifywait` in `waitfl`.
- In `genpath` and `editpath`, use `\0` as the line and keycode/path separator.
- Merge `repeat{line,null,str}` into `repeatstr`.
- Merge `factorise`, `gcd` and `sumbase` into `numsh`.

## Other
- Should [README.md](README.md) be rewritten to not include first person
  language?
- Rename `ffmw` subcommands to more sensible ones. For instance, the current
  name of the crop subcommand is very counter-intuitive.
- In scripts that acquire locks, release them with a trap in case the script is
  interrupted, killed, etc.
- Add `exit 0` at the end of all applicable scripts.
- Add explanations under shebangs.
- Add a file that explains every script. Mention this file in
  [README.md](README.md).
- Use `basename` and `dirname` instead of parameter expansions as they correctly
  deal with edge cases. If these edge cases are known not to be present, do use
  parameter expansions.
- Into the pathfinding suite, add a system for custom
  aliases/functions/variables depending on the current directory of the user.
  This could be done by modifying the currently provided `cd` function.
- Replace `printf` with `printf --`.
- In all scripts that parse arguments, ensure that help printing and other
  actions are done after validating the arguments. If the arguments could not be
  validated, exit with a non-zero code (after printing help if it is given).
  This way, the scripts can be integrated more easily with or within other
  scripts by allowing the arguments to be immediately validated without
  executing the script (like kdialog, which wraps the real kdialog, validating
  the arguments without doing the action).
- Scripts that require untrivial root access should not use `sudo`, but force
  the user to run the script as root. Scripts that require trivial root access
  should use `sudo` or `sudo -A` depending on whether they are graphical (for
  instance, if they use `dmenu`) or not.
- Register a tray icon using `yad` in applicable scripts.
  - Daemons could benefit well from this.
  - `tglapp` could benefit from this. A single tray icon when right clicked
    should show all `tglapp` applications. For this, a subscription and a server
    (like `lf`'s) system should be implemented.
- Show errors with `rofi -e "$message"` in applicable headless scripts.
- Use `getopt` if available. Use `getopts` otherwise. Be sure to check the
  `parseargs()` of individual scripts to see quirky parsing. Some scripts have
  non-standard parsing.
- Use `ulck` instead of manually releasing locks.
- Do not print error messages when `get*` fails in scripts as those already
  print errors to stderr.
- At this point perhaps rename the repository from `scripts` to `utils`?


# Low Priority

## Features
- Add adequate padding in `genrc`. (GNU?) `printf` has a padding parameter.
- In `pathfinding/v`, if the file is not found, try finding the file in *a few*
  depths.
- In `bright*`, fade the brightness.

## New Scripts
- Write a *layout manager* for dwm and/or bspwm. This manager would pop-up some
  sort of window showing (icon-ic) previews of the different available layouts.
  When one is selected, it would change the wm to that layout. You may want to
  use `rofi` or a `dialog` menu.

## Refactoring / Rewriting / Reworking
- In `weather`, pad the first and last lines to prevent the clashes of the two
  versions. If the whole output is padded, the lines do not look nice. See
  [this](https://www.unix.com/shell-programming-and-scripting/257005-how-add-extra-spaces-make-all-lines-same-length.html)
  for easy padding.
- In all C files (except, perhaps, those under `*/include/*`), move variable
  declarations as close as possible to the first usage, i.e. reduce their scope
  as much as possible. Still use C89 style declarations.
