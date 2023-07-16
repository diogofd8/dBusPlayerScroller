<h1 align="center">dBusPlayerScroller</h1>

A simple C++ program which grabs `mpris.MediaPlayer2` metadata through dBus and scrolls it on a window, created with the intention of being run with [polybar](https://github.com/polybar/polybar).
There are similar projects from which I took inspiration from, namely [jan146](https://github.com/jan146)'s [playerctl-scroller](https://github.com/jan146/playerctl-scroller) and [PrayagS](https://github.com/PrayagS)'s [polybar-spotify](https://github.com/PrayagS/polybar-spotify), please check them out.

## Installation
I did not create (yet) any installer for this. For now, simply clone this repository, compile it with `g++` and run the binary file. It can be moved to whichever directory, such as `~/.config/polybar/scripts/`.

```bash
git clone https://github.com/diogofd8/dBusPlayerScroller.git
cd dbusStringScroller
make -f makefile all
# Copy the binary to your preferred directory
mkdir -p ~/.config/polybar/scripts/
cp dbusStringScroller ~/.config/polybar/
```

## Configuration (polybar)
It can be launched from any `custom/script`, such as:

```ini
[module/spotify]
type = custom/script
tail = true
format = <label>
exec = $HOME/.config/polybar/scripts/dbusStringScroller
```

## Execution Parameters
While the default configuration (running the binary without extra arguments) is set to work with `spotify`, `dBusPlayerScroller` is meant to support any `mpris.MediaPlayer2` compatible player. The configuration options are as follows:

```console
dBusStringScroller [Options]
Options:
  -p | --player  [player name]
  -s | --speed   [scroll speed]
  -l | --length  [view length]
  -d | --debug
```
Scroll speed is in millisseconds, length corresponds to the dimensions (in character count) of the viewer window. I plan to add a metadata formatting option and a prefix/suffix option to format the output to any taste!

## Dependencies
- `git` `gcc` `dbus`
