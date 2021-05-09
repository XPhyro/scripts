# Bugs
- `sxhkd` does not release keys when cycling keymaps, fix.
- In `weather`, pad the first and last lines to prevent the clashes of the two versions. If the whole output is padded, the lines do not look nice. See [this](https://www.unix.com/shell-programming-and-scripting/257005-how-add-extra-spaces-make-all-lines-same-length.html) for easy padding.
- `tglapp -k restart-wallpaper -T off` does not work, fix.
- `lckget clplog` does not work, fix. The problem might be in `lckget` or `clplog`.

# Features
- Scratchpads in dwm are tagged by monitor. Make `dwmpad` recognise the monitor tags and initialise distinct (by monitor) locks according to the monitor tags.
- In `contexec`, show the output in a `dwmpad` and open the editor in the initialised terminal.
- After scratchpad setting is implemented in dwm, add support in `dwmpad`.
- Allow m to take input files. If an input file is given, mark the file; if not, mark the current working directory.
- In `contexec`, wrap each execution between printed headers. Also show the execution count.
- In `dwmpad`, allow setting the font size (not the whole font). This would be useful in scripts/programs like `weather`, `neomutt`, etc.
- Add options in `latexstp` to not open the source/output files.
- Add an option in `contexec` to open the editor. When this option is passed, open the editor in the current terminal and execute the file in a dwmpad.
- Allow assigning different images per display in `wallpaper`. `hsetroot` does not seem to support this.
- Add an option in dwm that would skip the acquisition of a lock if all locks are occupied. This could have two behaviours: open up a normal st session or execute the command without st. This could be made into two more options that would determine the behaviour.
- When setting a mark, ensure that the mark name does not contain %20 (i.e. space). Tabs, etc. are ok with the current setup. Alternatively, refactor the current setup to not use whitespace delimiters. A mechanism like in `getloc` and `getfl` could be implemented.
- Add support for regular files in mark scripts. A possible way could be to also add support for passed arguments for files in the marking process and check the file type; if it is a regular file, then the file could be marked in a different mark set. While using marks, first check if a directory mark exists. If there exists a directory mark, `cd` into it; else, open up the regular file for the given mark in `$EDITOR` if there is such a mark.
- Add adequate padding in `genrc`. (GNU?) `printf` has a padding parameter.
- Support non-terminal applications in `dwmpad` after scratchpad assignment is implemented in `dwm`.
- In `steammgr`, keep a database of application IDs scraped from [steamdb](https://steamdb.info/apps) and present the user with relevant options. This could be done in two ways: either show the relevant options by being command-aware, or first show a meta prompt for all, installed and in-library games, and then show the IDs (or show the meta options in the installed prompt, and show another prompt with the selected if all or in-library is selected).

# New scripts
- Create software alternatives to the `bright*` scripts that work on hardware level. Use `xr --output "$(mondef)" --brightness "$brightness"`. Create a more general version that automatically uses the hardware one if supported on hardware, else use the software one.
- Write a todo manager which would be a generalised form of the deadline reminder.
- Write a watson wrapper.
- Write a variant of `volappch` that toggles mute status.
- Migrate `dotfilesbak{,-sensitive}` into this repository, and simply symlink them to the original locations.
- Write `getfls` and `getlocs` that support multiple arguments unlike `getfl` and `getloc`. This might also be integrated in `getfl` and `getloc` in a lightweight manner.
- Write scripts that would: print the value for a mark, print the mark for a given value (if marks are always in expanded form, also expand the given value; else, ensure that the equivalent values are considered).
- Write a helper script that would make it easy for a script to implement hashmaps. Currently `getloc`, `getfl` and other scripts use directory/file structures as a workaround. This script would need to perform at least as well as the workaround, if not better, and be convenient to use.
- Make the `¬`, `@` and `g` functions into stand-alone scripts. Add wrapper functions in rc file that would cd to the stdout if there is any. In these functions, stderr should be directly printed and not captured.
- Write a *layout manager* for dwm and/or bspwm. This manager would pop-up some sort of window showing (icon-ic) previews of the different available layouts. When one is selected, it would change the wm to that layout. You may want to use `rofi` or a `dialog` menu.
- Write a library-ish thing to make it easy for scripts, especially those in terminal file managers, to implement short-term smart thrash functionality that is considerate of the disk usage of the thrash.
- Write a `dvtm` session manager using `dwmpad`, `abduco` and/or `screen`.

# Refactoring / Rewriting
- Integrate -pc option of dmenu into usable scripts.
- Optimise `kmcycle` and `setxkb`.
- Use `xsel` instead of `xclip`.
- Rewrite `lck`. See [this](https://stackoverflow.com/questions/185451/quick-and-dirty-way-to-ensure-only-one-instance-of-a-shell-script-is-running-at) and [this](http://mywiki.wooledge.org/BashFAQ/045).
- Make @ and ¬ into sourcable scripts instead of functions in .\*rc.
- In `dotfilesbak{,-sensitive}`, change the author and/or committer of the commits that are made by these scripts.
- Optimise `dwmpadinit`'s PID updating with `inotifywait`.
- Optimise `maps`.
- Rewrite `brightmute` in a more robust fashion that would not oftenly break.
- Rewrite `rangermpick` with `lf`.
- Consider `scrot` in `cpscr`.
- Make `dupe`, `getnewpath` and alike use GNU `--backup` syntax instead of continuous trailing underscores.
- Convert `open-steam` into a full Steam manager using the `steam://` protocol.

# Other
- Should [README.md](README.md) be rewritten to not include first person language?
- Move the *mark* family of scripts into a dedicated directory. So far, these are: `m`, `cm`, `¬`, `@`, `g`, `mg`.
- Instead of using `while ... done < "$file"`, use `< "$file" while ... done`.
- Get rid of the `printf "%s" "$sopt" | cut -c1` calls in `parseargs()`.
- Add `--` before passing files to applicable commands.
