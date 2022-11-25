<p align="center">
  <img src="https://github.com/Daxterapid/dungeoneration/raw/main/dungeon.png" width="40%" height="40%">
</p>

# dungeoneration
Simple terminal-based dungeon generation

## Motivation
I wanted to create a small roguelike game for an Arduino using a MAX7219 (small 8x8 LED matrix) and a joystick, so I created this small algorithm. The game is still a work-in-progress.

## Compiling
### GCC
Regular version:
```
gcc -o main main.c -O2
```
Old (worse) algorithm:
```
gcc -o main main.c -O2 -DUSE_WORSE_GENERATION
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
