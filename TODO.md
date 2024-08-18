# High Priority

## Security

- `eval`ing `getpath`, `xdotool` and other programs can lead to security
  problems if the scripts are run with elevated permissions. Warn the user and,
  for programs local to this repository, always install the `eval`ed programs as
  root to prevent tinkering.

## Bugs

- In `editpath`, the given path is not escaped, even if it is known to be a
  path not to be `wordexp`ed. Use `shell-escape` to escape the given path
  before adding it to the database if the path is known to be a path.
  - Perhaps also add a flag to signal no-escaping, and escape by default?
- `vstrcatbuf` and other similar functions do not return the new `bufsize` or edit
  the original variable.
  - Consecutive calls to these functions reallocates memory when it should not
    due to not updating `bufsize`. See `fizzbuzz` as an example.
- `make analyse` does not exit with non-zero code even if some analyses fail.
- Analyse target on GitHub does not work correctly.
- `wallpaper -x cron-gsettings` corrupts the wallpaper if run when not logged
  into GNOME.

## Features

- Add the ability to have default arguments via environment variables in
  `tglapp`.
- Support custom lock directory prefix in `dbutil.h`.
- Support custom lock directory prefix in `lck`.
- Implement for/while-else construct of Python in C++.
  [This](https://stackoverflow.com/a/25209781/8403070) could be used as a
  starting point.
- Add an option to override or append to the default clean-up process on
  toggling off in `tglapp`.
  - This would be useful for double-forking programs and or those that have
    weird process trees like Steam.
- Add support for dynamically queried paths in `getpath`. For instance:
  - `td` may be `printf "%s\n" TODO*(N) TODO.md | head -n 1` (zsh)
  - `rdm` may be `printf "%s\n" README*(N) README.md | head -n 1` (zsh)
  - `lc` may be `find . -mindepth 1 -maxdepth 1 -type f -iname "*license*" -print0 | head -z -n 1 | head -c -2` (sh)
- Make the interfaces of `std::*` (mostly) compliant with those of C++, even if
  it duplicates current interfaces.
- Make `std::*` easier to use in shells with aliases similar to `ensure`.
- Support printing all vectors, unordered maps, etc. with only program hash
  given. In other words, `[SYNTAX]` should be `[SYNTAX]?`, and aforementioned
  action should occur if no syntax is given.
- `getpath`: add options to print all keys, values and keys & values.
- Support globbing and regex in `exif-filter`.
- Support mounting/unmounting Android devices in `mountsel`/`umountsel`.
- `exif-filter`: support custom filter functions in place of the default
  `[ -n "$val" ] && [ "$flval" != "$val" ]`.
- `tglapp`: run applications in a `tmux` session.
  - Still capture stdout/stderr to temporary files.
    - Or, don't capture them, but query them from `tmux` if it is supported.
- `termctl`: include commands, subcommands, etc. in help dialog (or add a
  (command & subcommand)/flag for it)
- Support stdin in `gcc-otg`.
- Add support for arbitrary precision computation in `numsh`.
  - First create `linalg_mp.hpp`, then use it.
- `tabutils`:
  - Replace last tab if it is `about:blank` when pasting to the end.
  - Replace current tab if it is `about:blank` when pasting inline.
- Add alternate workflow targets for macOS on GitHub.
  - <https://docs.github.com/en/actions/using-github-hosted-runners/about-github-hosted-runners/about-github-hosted-runners#supported-runners-and-hardware-resources>
  - <https://github.com/actions/runner-images/blob/main/images/macos/macos-13-Readme.md>
  - <https://github.com/actions/runner-images/blob/main/images/macos/macos-13-arm64-Readme.md>
- Support `std::vector`, `std::unordered_map` and other future `std::`s in `numsh`.

## New Scripts

- Write a C program that takes in argv through argv and parsing configuration
  through stdin and outputs shell `eval`able code, then replace all applicable
  existing argument parsing in shell scripts with this program.
- Create a script for enabling/disabling/toggling/querying xinput devices. Use
  this script in `acpihandler`, `maptouch`, `tpcycle`, and other applicable
  scripts.
- Automate [PulseAudio/PipeWire-Pulse network
  streaming](https://gist.github.com/savegame/58ae5966c58a71fda5d3800b335eb2f5)
  with optional pair configuration over `ssh`.
- Merge `*maps` into a single C/C++ program.
- Write an audio waveform visualisation extension (in Lua) for `mpv`.
  - These could be useful:
    - <https://web.archive.org/web/20220925075135/https://www.securitronlinux.com/debian-testing/how-to-get-a-nice-waveform-display-with-the-mpv-media-player-on-linux/>
    - <https://web.archive.org/web/20220925075211/https://gist.github.com/matiaspl/8b1880456d10d582677aadb474d743b6>
- Write a script similar to `getpath`, but interactive.
  - `fzf` could probably work well here.
  - Should support early-out, similar to most LaTeX compilers.
    - If the only key starting with `abc` is `abcd`, `abc` should be enough to
      select `abcd`.
  - Should support filter & select without confirmation, similar to `lf`'s
    mapping system.
- Other `std` utilities:
  - `std::unordered_set`
  - `std::map`
  - `std::set`
- Write an init script to switch `nm-applet` ethernet presets depending on the
  network.
- Create two logging libraries that support colors (with automatic `isatty`
  detection) for C and C++ separately, using C and C++ streams respectively.
- Create a script that makes `dotnet format` easier to use.
  - It should be able to find the project/solution files and pass them to
    `dotnet format`.
  - It should handle special cases for Unity.
  - It should support both formatting all files in the solution and formatting
    only the files passed via the command-line (or also stdin?).
- Write a script that converts CBR to PDF and uses a PDF viewer to open the
  temporary file.
- Create a shell header, that, if included, makes the current script reload if
  the script file is touched.
  - Similar to `polybar`, `picom`, etc.
  - Use this header in daemons like `headsetcontrol2mpris`.
- Write a script that is key based like `tglapp` that caches the output of a
  command with the given key and re-uses the output.
  - Support output invalidation after a set (per key or global default) amount
    of time.
  - Maybe add implicit support for caching in `tglapp` or with pads (weather,
    etc.)?

## Refactoring / Rewriting / Reworking

- Rewrite `ffmw`.
  - Make it saner.
  - Share default options between subcommands.
  - Perhaps use C/C++/Python?
  - Use `std::vector` if using sh.
  - Use `-nostdin` and `file:` for `ffmpeg`.
  - Make `ffmpeg` less verbose.
  - Use segments (`-f segment`, `-segment_time TIME`, -segment_list `FILENAME.csv`).
    - See <https://video.stackexchange.com/questions/32297/resuming-a-partially-completed-encode-with-ffmpeg>
  - Use `parallel` with verbose progress & running commands.
- Optimise `breadth-find` to not search the same initial depths multiple times.
- Make `editpath` and related scripts as easy to use as `_m` and others.
- Rewrite the pathfinding suite with `std::unordered_map`, or with the
  to-be-implemented C/C++ `std` base library.
- Create a base C (or C++, depending on what features are required or heavily
  used (though C is preferred for future-compatibility)) library for creating
  other `std` utilities such as `std::vector` and `std::unordered_map`, then
  refactor `std::vector` and `std::unordered_map` using that library.
- Rewrite `tglapp` in C++, Go or Rust.
  - Much easier to support the other to-dos regarding `tglapp` in one of these
    languages rather than in shell.
- Export `align()` from `std::string` to `strutil.h` and use it from the
  header.
- Merge `fillline` and `fillterm` into `fillcol`.
- `prefix.sh`: don't query paths at runtime, make `.make` insert them in via
  `m4` during installation instead.
- Rewrite the pathfinding suite fully in C as a single program.
  - Don't require building a "database". Parse the configuration file on-the-go
    (or generate the database on-the-go and regenerate if mtime of database is
    less than the mtime of the configuration file).
  - Make mark support first-party and eliminate the distinction between
    permanent items and marks (or support setting marks as const).
  - Add built-in special marks such as `-`, `@`, etc. Make sure these marks are
    not caught or expanded by the POSIX Shell or mainstream shells like Bash
    or Zsh.
- Consider porting all execline scripts to shell for less dependencies.

## Other

- _Add support for installing single programs, plugins, etc._
- Documentation:
  - Online documentation via `github.io` & `codeberg.page`.
  - Offline documentation via man pages.
  - `-h` and/or `--help` flags.
  - For interpreted stuff and config files, brief explanation as comment at the
    beginning of the file.
  - Install man pages as compressed with `gzip`.
  - Examples in the repository
    - `sxhkd` examples for hotkey scripts
    - Script/excerpt examples for utilities & libraries
    - etc.
  - Table of everything in README.md with 3 fields: name, very brief
    description and (if possible) very brief example.
- At this point perhaps rename the repository from `scripts` to `utils`?
- Consider changing the extension of shell headers from `.sh` to `.shh`, `.hsh`
  or something similar.
- Add a commit hook that catches entries in the readme becoming broken or
  stale.
  - If entries become broken with the commit, the hook should:
    - Warn about the broken entries
    - Refuse the commit
  - Otherwise, it should:
    - Warn about entries possibly becoming stale
- Consider integrating `cpplint`.

---

# Normal Priority

## Bugs

- `ffmw decat` does not work as expected. When the time is in format `HH:MM:SS`,
  it seems to work. Maybe it requires this format? If so, add it in the help
  menu.
- Rewrite `parseargs()` without `while [ -n "$1" ]` to allow for empty
  arguments. Instead loop using the count of arguments or use `for i`.
- In `tglapp`, `--list=compact` prints stdout and stderr.
- `.make`: `.installed` grows indefinitely unless `uninstall` is issued, fix.
- `latexd` does not work with files containing `'`, fix.
- `std::string` segfaults if given alignment is not contained in the input.

## Features

- In `contexec`, show the execution count in the header.
- Add an option in `contexec` to open the editor. When this option is passed,
  open the editor in the current terminal and execute the file in a `$PAD`.
- In `steammgr`, keep a database of application IDs scraped from
  [steamdb](https://steamdb.info/apps) and present the user with relevant
  options. This could be done in two ways: either show the relevant options by
  being command-aware, or first show a meta prompt for all, installed and
  in-library games, and then show the IDs (or show the meta options in the
  installed prompt, and show another prompt with the selected if all or
  in-library is selected).
- Integrate `fzfp` of `stpv` into `f` and `_fd`.
- Integrate _synctex_ in `latexd`. See
  [this](https://www.math.cmu.edu/~gautam/sj/blog/20140310-zathura-fsearch.html)
  and [this](https://gist.github.com/vext01/16df5bd48019d451e078).
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
- In `tglapp`, if both `-k` and `$cmd` are given, and if an application with the
  same keycode is currently active, check that the given command is the same as
  the currently running application. If the commands do not check out, exit with
  error and do not toggle any applications.
- In `tglapp` add an option to auto-restart the application when it exits. Make
  this behaviour togglable between only non-zero codes and all codes.
- In `bspwm-show`, show thumbnails. To do this, use `rofi` instead of
  `execmenu`.
- Do not assume a font size in `rofifit`. The new calculation should still be
  generous such that it would not make the same mistake as `rofi` trimming
  _very_ short.
- In `tglapp`, add an option that disables auto-unlocking, allowing the user to
  review the stdout/stderr of the command.
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
- Into the pathfinding suite, add a system for custom
  aliases/functions/variables depending on the current directory of the user.
  This could be done by modifying the currently provided `cd` function.
- In `sandwichline`, add `-s, --same-dough` option to make the top and bottom
  of the sandwich the same when randomised.
- `std::string` should have a help message.
- Alternative to `make stats` that lists individual file statistics.

## New Scripts

- Write `getfls` and `getdirs` that support multiple arguments unlike `getfl`
  and `getdir`. This might also be integrated in `getfl` and `getdir` in a
  lightweight manner.

## Refactoring / Rewriting / Reworking

- Optimise `kmcycle` and `setxkb`.
- Use `xsel` instead of `xclip`.
- Consider `scrot` in `cpscr`.
- Make `-fin` options in `ffmw` global as they are used by every non-help
  subcommand.
- Rewrite `eln` using `mapexec`. This will allow the name to be arbitrary
  (except containing a newline).
- Rewrite `kasel` using `pgrep` and `kill --timeout`.
- Use `inotifywait` in `waitfl`.
- Merge `repeat{line,null,str}` into `repeatstr`.

## Other

- Rename `ffmw` subcommands to more sensible ones. For instance, the current
  name of the crop subcommand is very counter-intuitive.
- In scripts that acquire locks, release them with a trap in case the script is
  interrupted, killed, etc.
- Use `basename` and `dirname` instead of parameter expansions as they correctly
  deal with edge cases. If these edge cases are known not to be present, do use
  parameter expansions.
- Replace applicable `printf` invocations with `printf --`.
- In all scripts that parse arguments, ensure that help printing and other
  actions are done after validating the arguments. If the arguments could not be
  validated, exit with a non-zero code (after printing help if it is given).
  This way, the scripts can be integrated more easily with or within other
  scripts by allowing the arguments to be immediately validated without
  executing the script (like kdialog, which wraps the real kdialog, validating
  the arguments without doing the action).
- Scripts that do not require parameters from the invoking user should not use
  `sudo` and should require the script to be run as root (by asserting it
  actively _if necessary_).
- Scripts that use `sudo` should use `sudo`/`sudo -A` if it has a CLI/GUI.
- Register a tray icon using `yad` in applicable scripts.
  - Daemons could benefit well from this.
  - `tglapp` could benefit from this. A single tray icon when right clicked
    should show all `tglapp` applications. For this, a subscription and a server
    (like `lf`'s) system should be implemented.

---

# Low Priority

## Bugs

- The following `stest` usages yield different results. They should not. The
  second one yields the correct results. There might be some cache corruption or
  clean-up issue related to the `libmagic` or `file` (via `fork`/`execlp`)
  usages.
  1. `find . -print0 | sort -z | xargs -r0 stest -z -m video -- | sort -z`
  2. `find . -print0 | sort -z | xargs -r0 -n 1 stest -z -m video -- | sort -z`
  - _Update_: They now yield the same result. Maybe there is some sort of race
    condition in the first case?

## Features

- Generalise `vector`:
  - From `1D->1D` to `ND->MD`.
    - Maybe even support variadic vectors like: `{{1, 2}, {3}, {4, 5, 6}}`.
      Values can remain as strings.
  - Support `key->value` and `index->value` at the same time. Key can just be
    strings.

## New Scripts

- After `vector` is generalised, write a shell library using `vector` to parse
  arguments. Or just create another C/C++ program for this.
  - Ideally, it would
    - require very little or no boilerplate code.
    - support parsing without a loop in the shell script.
    - output error-checked `eval`able variables similar to `getpath`.

## Refactoring / Rewriting / Reworking

- In all C files (except, perhaps, those under `*/include/*`), move variable
  declarations as close as possible to the first usage, i.e. reduce their scope
  as much as possible. Still use C89 style declarations.
- Optimise `database.h` to do less allocations.
  - Reuse built paths.

---

# Archive

## Security

- Do not `eval` the output of `headsetcontrol` in `polybar-headsetcontrol` in
  case the interface changes.
  - Maybe send a PR for an `-e` (for eval) flag to `headsetcontrol`?

## Bugs

- `acpihandler` does not handle already-off keyboard/touchpad correctly on
  PROG1 event.

## Features

- In `bspwmpad`, if a slot is requested and the requested slot is occupied,
  check if the occupier is alive. If a slot is not requested and all slots are
  occupied, check all of the slots for whether each occupier is alive.
- In `bspwmpad`, add an option to start in some desktop.
- In `bspwmpad`, support non-automated non-incrementing keycodes in addition to
  numbers.
- Support anonymous (non-numbered) pads in `bspwmpad`.
  - In `latexd`, if `$TABBED_XID` is set and non-empty, use request an
    anonymous pad.
- In `bspwmpad`, allow setting the font size (not the whole font). This would be
  useful in scripts/programs like `weather`, `neomutt`, etc.
- Add options in `latexd` to not open the source/output files.
- Add an option in `bspwmpad` that would skip the acquisition of a lock if all
  locks are occupied. This could have two behaviours: open up a normal st
  session or execute the command without st. This could be made into two more
  options that would determine the behaviour.
- In `bspwmpad`, add an option to use `tglapp`.
- In `tglapp`, add an option to use `bspwmpad`. If this option is given, when
  toggling the application, toggle the hidden status of the pad if it is on.
- In `latexd`, allow passing arguments to `bspwmpad`.
- In `bspwmpad`, support killing the command running in the given pad number.
- In `bspwmpad`, support killing the command running in the given pad number and
  replacing it. This function should require a valid and non-zero `-n` to be
  passed.
- In `bright*`, fade the brightness.
- Make the `kdialog` wrapper cross-compatible with other dialogue applications
  such as Zenity. Create wrappers for these newly supported applications that
  `exec` the `kdialog` wrapper.
  - Or, create a generic base file dialogue application that uses `lf` and
    create wrappers for other file dialogue applications that `exec` this
    generic base application.
- Support `fzf` in `selfl`.
  - By default, automatically determine whether to use `fzf` or `$MENU`
    depending on `[ -t 0 ]`.
- Add adequate padding in `genrc`. (GNU?) `printf` has a padding parameter.
- In `pathfinding/v`, if the file is not found, try finding the file in _a few_
  depths.
- Show errors with `rofi -e "$message"` in applicable headless scripts.
  - Maybe create a helper script for this?
- In `contexec`, show the output in a `$PAD` and open the editor in the
  initialised terminal.
- In `tglapp`, support terminating the X window instead of the run process.
- Currently, if `fillline` is included in an on-terminal-initialisation
  pipeline, the line might not be filled fully. This is not per se a bug on the
  `fillline` end, but it would be nice to mitigate it. Add an option to wait a
  tiny bit to allow for the terminal window to initialise and settle to its
  permanent/semi-permanent row/column configuration. Add another option to wait
  for the terminal row/column values to change. Add another option to limit the
  waiting time of the previous action, so it does not end up waiting
  indefinitely.

## New Scripts

- Migrate `dotfilesbak{,-sensitive}` into this repository, and simply symlink
  them to the original locations.
  - Support multiple systems:
    - Support non-shared and shared files.
    - Support "static" files, where those files are owned by a system and may
      not be shared.
    - If we do not support static files, or more than two systems with partial
      sharing, these should be easily achievable with git branches. Otherwise,
      possible implementations could be:
      - A directory structure with a flags hashmap,
      - A directory structure with the shared files being symlinked,
      - Etc.
    - Easiest solution that would support multiple systems, synchronous usage
      across multiple systems and static & shared files is most likely either
      multiple repositories or multiple copies of the same repository with
      distinct branches checked-out.
      - Have a root directory with necessary configuration files.
        - This would also make it easy to support public/private dotfiles more
          easily.
