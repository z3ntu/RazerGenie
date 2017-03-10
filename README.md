# \<insert name here>

~~KConfig Module for [razer-drivers by @terrycain](https://github.com/terrycain/razer-drivers). This project is supposed to be used for configuring Razer devices in the KDE system settings.~~
Standalone Qt application for configuring your Razer devices under GNU/Linux. **A new name is in search!**

## How to compile
```
mkdir build && cd build
cmake ..
make
./src/kcm_razerdrivers
# You could install it with 'make install' but that's not recommended as files get left in the filesystem (use a distribution package, if available!).
```

## Dependencies
* Qt5 (+ the modules: DBus, Network & Xml)
* kwidgetsaddons
* razer-drivers
* cmake
* extra-cmake-modules

### Ubuntu
```
sudo apt install cmake extra-cmake-modules qt5-default libkf5configwidgets-dev
```

### Arch Linux
```
sudo pacman -S --needed cmake extra-cmake-modules qt5-base kconfigwidgets
```

## Current status
* librazer shared library (will be split from this repo when it is halfway usable)
* Displays your devices
* Has a nice (the official) icon
* Can set the brightness of all devices
* Can set some (you could say "most") effects
* Display correct "thumbnails" for devices
* Can set the DPI of mice

## TODO
* Implement keyboard gui (individual lighting etc)
* Make images in the device list bigger (probably is pretty complicated with the KPageWidget I am using)
* **Much** more
