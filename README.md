# CHIP8 Emulator

> CHIP-8 is an interpreted programming language, developed by Joseph Weisbecker. Initially used on the COSMAC VIP and Telmac 1800 8-bit microcomputers in the mid-1970s, it allowed video games to be easily programmable.

![alt text](https://github.com/jankdc/cee/raw/master/data/images/invaders.png "Space Invaders Game in Chip8")

## Dependencies

- GLFW3 (3.1.x)
- GLEW (1.12.x)
- GLM (0.9.6.x)
- Premake4 (Build System)

## Build

```bash
cd cee
premake4 gmake
make config=debug
```

## Usage

Currently, opening the binary requires that it must be opened,
whilst cd'd via the root of the project as there are some file paths
that are dependent on it (e.g. shader resources).

```bash
cee FILE_PATH
```

## Example

```bash
cd cee
./bin/debug/cee ./data/programs/INVADERS
```

## TODO LIST

- [ ] Add beeping sound (currently no sound).
- [ ] Add controls to frame rate.

## Games

The following games are distributed with the interpreter:

- 15PUZZLE
- BLINKY
- BLITZ
- BRIX
- CAVE
- CONNECT4
- GUESS
- HIDDEN
- INVADERS
- KALEID
- MAZE
- MERLIN
- MISSILE
- PONG
- PONG2
- PUZZLE
- SYZYGY
- TANK
- TETRIS
- TICTAC
- UFO
- VBRIX
- VERS
- WALL
- WIPEOFF

You can find them in the *data/programs* folder.

## License

The code is available under the MIT license.
