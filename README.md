# Musializer Plugin
Visualizer plugin inspired by [Tsoding's Musializer](https://github.com/tsoding/musializer) with in C++ using JUCE.

## Demo
https://github.com/user-attachments/assets/bc821e71-e685-4b1a-b64b-f86ef9ee05bc

## Usage
Get the appropriate binary from the [releases](https://github.com/ameyakakade/musializer-plugin/releases) or build it yourself. Move the file to the folder containing your plugins.

#### MacOS
Plugins are usually in /Library/Audio/Plug-Ins/ Move the au and vst3 in the appropriate folders. If daw says "Drummock cannot be opened" this is because the MacOS binaries are not notarized. To fix run the following commands in the terminal:
```bash
# For VST3
sudo xattr -cr /Library/Audio/Plug-Ins/VST3/Musializer\ Plugin.vst3

# For AU
sudo xattr -cr /Library/Audio/Plug-Ins/Components/Musializer\ Plugin.component
```
## Why does this exist?
Musializer has really sweet graphics which I wanted inside my DAW while making music.

## What is different?
Instead of using `drawRectangle` functions from raylib (or JUCE in this case) to draw the bars, everything is drawn using a single fragment shader. The heights of the bars are passed in as a texture. Beware, the shader isn't particularly efficient.

## Plagiarism
This project uses pieces from Tsoding's Musializer, mainly the FFT implementation.
Google Gemini was used to provide boilerplate for getting OpenGL working in JUCE.

## Building 
This project uses CMake. Make sure you download JUCE and it's dependencies if any.
```bash
cmake -B build -G Xcode
cmake --build build
```

