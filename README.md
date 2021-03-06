# Tiny Mod Manager
Tiny Mod Manager (tmm) is a simple console based mod manager. It uses symlinks to quickly install and uninstall mods from an external directory. This program was designed primarily for use with Linux, but you can build it for Windows too with MinGW.

This software is still in alpha. Due to potential bugs, by using this program, you may run the risk of corrupting your game directory.

# Installation
To build and install tmm on Linux, run:
```
make install
```
You can also just run ```make``` and put the executable directly in the game directory and run it from there.

# Usage
To initialize tmm for a single game, go to that game's data directory and run:
```
tmm
```
This will generate a ```mods/``` directory and a ```mods.txt``` file. 

### Installing a mod
To install a mod, place a directory containing all the mod files into ```mods/``` and then rerun:
```
tmm
```
Note that a mod's directory structure must match that of the game's data directory. For instance, if you wanted to install a file to ```gameroot/textures/image.png```, the mod directory would have to look like ```mymod/textures/image.png```. Most mods will do this by default; some do not.

Tmm will automatically maintain a list of all installed mods in ```mods.txt```. It installs the mods from top down. In the event of a file conflict the lower listed mod will overwrite the upper listed mod. You can manually re-arrange this list to configure mod install order. Any invalid list entries will be deleted on the next run of tmm.

### Uninstalling a mod
To uninstall a mod, move its directory out of the ```mods/``` and run:
```
tmm
``` 
A mod can also be uninstalled by prepending a ```/``` character to it's entry in mods.txt effectively "commenting it out".
