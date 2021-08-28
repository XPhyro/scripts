# Bugs
- `ffmw decat` does not work as expected, fix. When the time is in format `HH:MM:SS`, it seems to work. Maybe it requires this format? If so, add it in the help menu.
- Rewrite `parseargs()` without `while [ -n "$1" ]` to allow for empty arguments. Instead loop using the count of arguments or use `for i`.

# Features
- In `contexec`, show the output in a `$PAD` and open the editor in the initialised terminal.
- Allow `m` to take input files. If an input file is given, mark the file; if not, mark the current working directory. Similarly, support those files in `@` for editing.
- In `contexec`, wrap each execution between printed headers. Also show the execution count.
- In `dwmpad` and `bspwmpad`, allow setting the font size (not the whole font). This would be useful in scripts/programs like `weather`, `neomutt`, etc.
- Add options in `latexstp` to not open the source/output files.
- Add an option in `contexec` to open the editor. When this option is passed, open the editor in the current terminal and execute the file in a `dwmpad` or `bspwmpad`.
- Allow assigning different images per display in `wallpaper`. `hsetroot` does not seem to support this.
- Add an option in `dwmpad` and `bspwmpad` that would skip the acquisition of a lock if all locks are occupied. This could have two behaviours: open up a normal st session or execute the command without st. This could be made into two more options that would determine the behaviour.
- When setting a mark, ensure that the mark name does not contain %20 (i.e. space). Tabs, etc. are ok with the current setup. Alternatively, refactor the current setup to not use whitespace delimiters. A mechanism like in `getdir` and `getfl` could be implemented.
- Add support for regular files in mark scripts. A possible way could be to also add support for passed arguments for files in the marking process and check the file type; if it is a regular file, then the file could be marked in a different mark set. While using marks, first check if a directory mark exists. If there exists a directory mark, `cd` into it; else, open up the regular file for the given mark in `$EDITOR` if there is such a mark.
- Add adequate padding in `genrc`. (GNU?) `printf` has a padding parameter.
- Support non-terminal applications in `dwmpad` after scratchpad assignment is implemented in `dwm`.
- In `steammgr`, keep a database of application IDs scraped from [steamdb](https://steamdb.info/apps) and present the user with relevant options. This could be done in two ways: either show the relevant options by being command-aware, or first show a meta prompt for all, installed and in-library games, and then show the IDs (or show the meta options in the installed prompt, and show another prompt with the selected if all or in-library is selected).
- Integrate `fzfp` of `stpv` into `f` and `_fd`.
- Integrate *synctex* in `latexstp`. See [this](https://www.math.cmu.edu/~gautam/sj/blog/20140310-zathura-fsearch.html) and [this](https://gist.github.com/vext01/16df5bd48019d451e078).
- Implement a no-quoting option in `tglapp` like the inverse of the one that exists in `mapexec`.
- In `bright*`, fade the brightness.
- In `pathfinding/v`, if the file is not found, try finding the file in *a few* depths.
- `ffmpeg` fails to parse the filename (why does it even parse it?) if it contains one of the following characters `:,`. Make `ffmw` create a symlink pointing to the input file(s) to mitigate the issue.
- In `tglapp`, add support for different commands depending on whether the application is open or not. This would be useful in many scenarios, including toggling an application wrapped with `screen`.
- Implement `-l, --list` in `tglapp`.
- Add an option to start applications in a `screen` named `"$keycode"` in `tglapp`. Also add an option to overwrite the `screen` name.
- Support `--long-option=VALUE` in `ffmw`, `selfl` and `dwmpad`.
- Support `-l LIMIT, --limit LIMIT` global option in `ffmw` to limit the CPU usage of `ffmpeg` using `cpulimit`.
- In `tglapp`, add support for force restarting applications via only a keycode. To do this, read the command to start from `*/cmd`.
- In `tglapp`, add `by-cmd`, `by-kc` (by-keycode), `by-hash` and `by-pid` sub-directories like `/dev/*/by-*`.
- Add usage statistics to `g`, `e` and `i` to rename keycodes to more efficient ones. Be sure to differentiate between scripted calls and interactive calls, this is probably a non-issue as the keycodes would be vastly different.
- Use `get*unsafe` in `_[ge]*`.
- Make the mark '¬' not shared between terminals, but still saved after the terminal is closed. Load the value only at initialisation. In the interactive shell side, before exiting, `cp -f "$somedir/$$" "$somedir/0"` and when initialising, `cp "$somedir/0" "$somedir/$$"` can respectively be used to save and load.
- In `tglapp`, implement `-F, --fork` using `setsid --fork -- "$realshell" -c`.
- In `tglapp`, if both `-k` and `$cmd` are given, and if an application with the same keycode is currently active, check that the given command is the same as the currently running application. If the commands do not check out, exit with error and do not toggle any applications.
- In `*pad`, add an option to use `tglapp`.
- In `tglapp`, add an option to use `*pad`. If this option is given, when toggling the application, toggle the hidden status of the pad if it is on.
- In `tglapp` ad an option to auto-restart the application when it exits. Make this behaviour togglable between only non-zero codes and all codes.
- In `bspwm-show`, show thumbnails. To do this, use `rofi` instead of `execmenu`.
- Do not assume a font size of 16 in `rofifit`. The new calculation should still be generous such that it would not make the same mistake as `rofi` trimming short.

# New scripts
- Create software alternatives to the `bright*` scripts that work on hardware level. Use `xr --output "$(mondef)" --brightness "$brightness"`. Create a more general version that automatically uses the hardware one if supported on hardware, else use the software one.
- Finish `todo`.
- Write a variant of `volappch` that toggles mute status.
- Migrate `dotfilesbak{,-sensitive}` into this repository, and simply symlink them to the original locations.
- Write `getfls` and `getdirs` that support multiple arguments unlike `getfl` and `getdir`. This might also be integrated in `getfl` and `getdir` in a lightweight manner.
- Write scripts that would: print the value for a mark, print the mark for a given value (if marks are always in expanded form, also expand the given value; else, ensure that the equivalent values are considered).
- Write a helper script that would make it easy for a script to implement hashmaps. Currently `getdir`, `getfl` and other scripts use directory/file structures as a workaround. This script would need to perform at least as well as the workaround, if not better, and be convenient to use.
- Write a *layout manager* for dwm and/or bspwm. This manager would pop-up some sort of window showing (icon-ic) previews of the different available layouts. When one is selected, it would change the wm to that layout. You may want to use `rofi` or a `dialog` menu.
- Write a library-ish thing to make it easy for scripts, especially those in terminal file managers, to implement short-term smart thrash functionality that is considerate of the disk usage of the thrash.
- Timer, chronometer, alarm.
- Implement a crop subcommand in `ffmw`.
- Write a version of `v` (from .zshrc) that has keys as in the pathfinding suite depending on the directory. Say, one executes `vm t dotfilesbak-tick` in `"$HOME/.dotfiles"`. When they execute `"$scriptname" t`, `dotfilesbak-tick` is opened with `"$EDITOR"`. Alternatively, this behaviour could be integrated in the currently existing mark functionality either by prioritising directory-specific marks or by making the user prepend the key by a character, say \_.
- Write an `stest` wrapper that supports the negation of partial operators. As an example `stest -x ! -d` should return all files that are executable but are not directories. Alternatively, have a syntax like `stest -x !-d` or the similar.
- Write a script similar to `bspwmpad` that would register/deregister the focused node if the key (1-9) is empty, else it will deregister that key; then, write a script that would hide/show these nodes per key. Key 0 should be similar to the key 0 of `bspwmpad`.
- Write a script to check an sxhkdrc file (by default `"$HOME/.config/sxhkd/sxhkdrc"`) that would check for duplicate hotkeys, *not* assuming the modifiers are in a particular order, and print the hotkeys along with their commands.
- Using `mapexec`, write a batch renaming tool that passes the name through `stat --printf=` if the line starts with ``.
- Write a `rofi` wrapper that dynamically sets the width as in the following excerpt from `yankunicode`: `rofi -dmenu -font 'JetBrainsMono 16' -width -"$(($(wc -L -- "$fl" | cut -d' ' -f1) + 2))"`.

# Refactoring / Rewriting / Reworking
- Integrate -pc option of dmenu into usable scripts.
- Optimise `kmcycle` and `setxkb`.
- Use `xsel` instead of `xclip`.
- In `dotfilesbak{,-sensitive}`, change the author and/or committer of the commits that are made by these scripts.
- Optimise `dwmpadinit` and `bspwmpadinit`'s PID updating with `inotifywait`.
- Rewrite `brightmute` in a more robust fashion that would not oftenly break.
- Consider `scrot` in `cpscr`.
- Make `dupe`, `getnewpath` and alike use GNU `--backup` syntax instead of continuous trailing underscores.
- Make `-fin` options in `ffmw` global as they are used by every non-help subcommand.
- Make `xins` use IFS= or IFS=\n.
- Make `mvloc` have the same syntax as `mv`. That is, `mv file1 file2 ... fileN location`. `eval "$#=\"$(getdir $$#)\""` could be used to do so.
- Rewrite `getnewname` and `getnewpath` to be compatible with GNU's `--backup=numbered`.
- Rewrite `eln` using `mapexec`. This will allow the name to be arbitrary (except containing a newline).
- Rewrite `kasel` using `pgrep` and `kill --timeout`.
- In `weather`, pad the first and last lines to prevent the clashes of the two versions. If the whole output is padded, the lines do not look nice. See [this](https://www.unix.com/shell-programming-and-scripting/257005-how-add-extra-spaces-make-all-lines-same-length.html) for easy padding.

# Other
- Should [README.md](README.md) be rewritten to not include first person language?
- Rename `ffmw` subcommands to more sensible ones. For instance, the current name of the crop subcommand is very counter-intuitive.
- In scripts that acquire locks, release them with a trap in case the script is interrupted, killed, etc.
- Add `exit 0` at the end of all scripts.
- Add explanations under shebangs.
- Add a file that explains every script. Mention this file in [README.md](README.md).
- Use `basename` and `dirname` instead of parameter expansions as they correctly deal with edge cases. If these edge cases are known not to be present, do use parameter expansions.
- Into the pathfinding suite, add a system for custom aliases/functions/variables depending on the current directory of the user. This could be done by modifying the currently provided `cd` function.
- Replace `printf` with `printf --`.
- In all scripts that parse arguments, ensure that help printing and other actions are done after validating the arguments. If the arguments could not be validated, exit with a non-zero code (after printing help if it is given). This way, the scripts can be integrated more easily with or within other scripts by allowing the arguments to be immediately validated without executing the script (like kdialog, which wraps the real kdialog, validating the arguments without doing the action).
- Scripts that require untrivial root access should not use `sudo`, but force the user to run the script as root. Scripts that require trivial root access should use `sudo` or `sudo -A` depending on whether they are graphical (for instance, if they use `dmenu`) or not.
- Use `cut` instead of `awk` where applicable. For instance, replace `awk '{print $1}'` with `cut -d' ' -f1` if `-d' '` suffices.
- Merge `bspwmpad{,init}` and `dwmpad{,init}` into `wmpad{,init}`. `wmpad{,init}` must work in both `bspwm` and `dwm` and should work in other WMs or DEs excluding the show/hide functionality.
- For easy debugging, in every shell script, add `eval "$(setverbose)"`. `setverbose` should check for the value of an environmental variable, say `SHELL_VERBOSE`, and print `set -x` if it is 1. This way, debugging nested scripts will be easier. Be sure not to include this in stderr-sensitive scripts (which there is none?).
