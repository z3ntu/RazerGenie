# RazerGenie

Qt application for configuring your Razer devices under GNU/Linux.

RazerGenie is using [OpenRazer](https://openrazer.github.io) for providing control over Razer peripherals.

![Screenshot of the RazerGenie main window](https://z3ntu.github.io/RazerGenie/screenshots/mainwindow.png)
![Screenshot of the RazerGenie custom editor](https://z3ntu.github.io/RazerGenie/screenshots/customeditor.png)

## Installation
Packages are available for these distributions:
* **Arch Linux:** you can install the package from the AUR: [razergenie](https://aur.archlinux.org/packages/razergenie/) or [razergenie-git](https://aur.archlinux.org/packages/razergenie-git/)
* **Debian/Fedora/openSUSE/Ubuntu:** [Download from OBS (openSUSE Build Service)](https://software.opensuse.org//download.html?project=hardware%3Arazer&package=razergenie)
* **Solus:** Install `razergenie` via the Software Center
* **Flatpak:** RazerGenie is available on [Flathub](https://flathub.org/apps/details/xyz.z3ntu.razergenie)!

Before installing RazerGenie please follow the [instructions on how to install OpenRazer](https://openrazer.github.io/#download) as you might hit unexpected problems otherwise.

If you are using a distribution not listed here please let me know and I'll try to make a package for that distribution!

## How to compile
This is a quick and easy way to test the RazerGenie without installing it or to test the master branch.
```
mkdir build && cd build
meson ..
# If you want to enable experimental (=unfinished) features
meson configure -Denable_experimental=true
ninja
./src/razergenie
# You could install it with 'ninja install' but that's not recommended as files get left in the filesystem (use a distribution package, if available!).
```

## Bugs
If your device is not detected by RazerGenie and the device is [supported by OpenRazer](https://github.com/openrazer/openrazer/blob/master/README.md#device-support), it will most likely be an issue with your installation or configuration of OpenRazer. View the ['Troubleshooting' page in the OpenRazer Wiki](https://github.com/openrazer/openrazer/wiki/Troubleshooting) for more information.

Functional or visual issues in RazerGenie should be opened in this repository.

## Translations
RazerGenie supports multiple languages! If your language isn't yet included or you want to improve existing translations, please take a look at the ['Translations' Wiki page](https://github.com/z3ntu/RazerGenie/wiki/Translations).
