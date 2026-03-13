# Musializer Plugin
Visualizer plugin inspired by [Tsoding's Musializer](https://github.com/tsoding/musializer) with in C++ using JUCE.

## Usage
Get the appropriate binary from the [releases](https://github.com/ameyakakade/musializer-plugin/releases) or build it yourself.

## Demo
https://github.com/user-attachments/assets/bc821e71-e685-4b1a-b64b-f86ef9ee05bc

## Building 
This project uses CMake.
```bash
cmake -B build -G Xcode
cmake --build build
```
## Why does this exist?
Musializer has really sweet graphics which I wanted inside my DAW while making music.

## What is different?
Instead of using `drawRectangle` functions from raylib (or JUCE in this case) to draw the bars, everything is drawn using a single fragment shader. The heights of the bars are passed in as a texture. Beware, the shader isn't particularly efficient.

## Plagiarism
This project uses pieces from Tsoding's Musializer, mainly the FFT implementation.
Google Gemini was used to provide boilerplate for getting OpenGL working in JUCE.
