# Simple blackjack in your terminal

## Implementation
Simple blackjack. Press 'h' to hit, press any other key to stick, press 'q' to quit or Ctrl+c.

No splitting or doubling down. No betting.

Draws count as player win.

## Requirements
Requires ncurses library. Debian derivates: `sudo apt install libncurses-dev` or something similar.

## Build
```
make clean && make all
```

## Run
Launch a terminal emulator with size at least 6 rows by 61 columns, then run `./main`