# To do

## Bugs


## Features
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

## New scripts
- Create software alternatives to the `bright*` scripts that work on hardware level. Use `xr --output "$( mondef )" --brightness "$brightness"`. Create a more general version that automatically uses the hardware one if supported on hardware, else use the software one.
- **Write a todo manager which would be a generalised form of the deadline reminder.**
- Write a watson wrapper.
- Write a variant of `volappch` that toggles mute status.
- Migrate `dotfilesbak{,-sensitive}` into this repository, and simply symlink them to the original locations.

## Refactoring / Rewriting
- Integrate -pc option of dmenu into usable scripts.
- Rewrite `tglapp` in C. It proves very useful in a lot of situations, it could do with the performance boost even though it usually takes about 10ms to execute (empty subshell takes 4ms!).
- Optimise `kmcycle` and `setxkb`.
- Use `xsel` instead of `xclip`.
- Rewrite `lck`. See [this](https://stackoverflow.com/questions/185451/quick-and-dirty-way-to-ensure-only-one-instance-of-a-shell-script-is-running-at) and [this](http://mywiki.wooledge.org/BashFAQ/045).
- Should `-a` be the default in `tglapp`?
- Make @ and ¬ into sourcable scripts instead of functions in .\*rc.
- In `dotfilesbak{,-sensitive}`, change the author and/or committer of the commits that are made by these scripts.
- Optimise `dwmpadinit`'s PID updating with `inotifywait`.
- Optimise `maps`.
