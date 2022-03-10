# One Lone Coder - Games

A collection of games and demos written in C++.

## About

All projects where coded based on videos and tutorials by David Barr, also known as [OneLoneCoder](https://www.youtube.com/c/javidx9), especially the Code-it-yourself [video series](https://www.youtube.com/playlist?list=PLrOv9FMX8xJE8NgepZR1etrsU63fDDGxO).

## Technologies

- C++
- Win32 API
- Visual Studio Community Edition
- Most projects use one of the following engines/libraries:
  - [olcConsoleGameEngine](https://github.com/OneLoneCoder/videos/blob/master/olcConsoleGameEngine.h)
  - [olcPixelEngine](https://github.com/OneLoneCoder/olcPixelGameEngine)
  - [olcNoiseMaker](https://github.com/OneLoneCoder/synth)

## Projects

All projects are *work-in-progress* and would need some extra-work to become fully playable games, but they all should build and run proberly.

Project        | Type       | Descrition
---------------|------------|--------------
ArcadeRacing   | Game       | Prototype for an arcade racing game in the style of 16-bit games like Top Gear
AStar          | Algorithm  | Implementation for the A-Star path searching algorithm
Asteroids      | Game       | Clone of the classic game Asteroids
CarCrime       | Engine     | Engine for a car game similar to the first GTA games
CaveDiver      | Game       | Simple console/terminal game where you control a ship diving into a cave and you must avoid colliding with the cave walls
ConsoleWebCam  | Demo       | Tech demo that displays image capture from a webcam in a text terminal
Conway         | Demo       | Implementation of Conways' Game of Life
Engine3D       | Engine     | Software rendered 3D game engine
FPS            | Engine     | Raycasting first person shooter game engine (similar to Wolf3D)
Frogger        | Game       | Clone of the classic game Frogger
MatrixRain     | Demo       | The Matrix character code "rain"
MazeGen        | Algorithm  | Maze generator algorithm and visualization
PerlinNoise    | Demo       | Demo for perlin noise generation algorithm
Platformer     | Engine     | Engine for a platformer game similar to 8-bit platformers like NES's Super Mario
RPG            | Engine     | Engine for a RPG game
RTS            | Demo       | Simple demo of an RTS game (select and move troops with mouse)
Snake          | Game       | Clone of the classic game Snake
SoundSynth     | Demo       | Simple keyboard sound synthesizer
Splines        | Algorithm  | Implementation of *splines* algorithm
Tetris         | Game       | Clone of the classic game Tetris
Worms          | Game       | Clone of the classic game Worms

## Compiling

All projects were setup using Visual Studio Community Edition 2017, but should compile without any issues in newer versions of VS.

The projects using olcPixelEngine should also compile with [other compilers](https://github.com/OneLoneCoder/olcPixelGameEngine/wiki/Compiling-on-Windows-with-Other-Compilers) or under [Linux](https://github.com/OneLoneCoder/olcPixelGameEngine/wiki/Compiling-on-Linux) with minor adjustments.

