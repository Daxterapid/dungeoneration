# dungeoneration
Simple terminal-based dungeon generation

## Motivation
I wanted to create a small roguelike game for an Arduino with the MAX7219 (a small 8x8 LED matrix) and a joystick, so I created this small algorithm. The game is still a work-in-progress.

## Compiling
With GCC (the `-O2` is optional):
```
gcc -o main main.c -O2
```

## Usage

### Windows
Generate and display a dungeon:
```
main
```
Display statistics:
```
main stat
```

### Linux
Generate and display a dungeon:
```
./main
```
Display statistics:
```
./main stat
```

## Bugs
If you find any bugs or improvements, please open an issue or create a pull request!
