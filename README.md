<!--
SPDX-License-Identifier: GPL-3.0-or-later
SPDX-FileCopyrightText: 2016 Luca Weiss <luca@lucaweiss.eu>
-->

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
meson setup builddir
meson compile -C builddir
./builddir/src/razergenie
# You could install it with 'meson install -C builddir' but that's not
# recommended # as the installation prefix has to be adjusted and files can
# get left in the filesystem.
# Use the package from your package manager whenever possible!
```

## Bugs
If your device is not detected by RazerGenie and the device is [supported by OpenRazer](https://github.com/openrazer/openrazer/blob/master/README.md#device-support), it will most likely be an issue with your installation or configuration of OpenRazer. View the ['Troubleshooting' page in the OpenRazer Wiki](https://github.com/openrazer/openrazer/wiki/Troubleshooting) for more information.

Functional or visual issues in RazerGenie should be opened in this repository.

## Translations
RazerGenie supports multiple languages! If your language isn't yet included or you want to improve existing translations, please take a look at the ['Translations' Wiki page](https://github.com/z3ntu/RazerGenie/wiki/Translations).


### Additional device controls in this fork

A dedicated Scroll wheel tab includes tactile/free-spin mode, Smart-Reel and scroll acceleration when the daemon advertises both getter and setter methods. Opening or refreshing the page only reads hardware state. While the Scroll wheel tab is visible, asynchronous reads update mode, Smart-Reel and acceleration once per second to follow hardware-button or external-tool changes. Background responses do not overwrite a newer manual change or an open dropdown. Failed writes display an error and restore the controls to the latest readback; unavailable readings disable the affected controls until Refresh succeeds.

For constant free-spin, disable Smart-Reel. Charging status displays as unknown when the daemon does not provide it or the query fails. Devices with reactive lighting expose a duration selector (500, 1000, 1500 or 2000 ms); this is independent of breathing rate, which the standard API does not expose.

These additions use the existing daemon D-Bus API and work with USB or Bluetooth backends advertising the same methods. The scroll controls do not require a patched libopenrazer.

Optional isolated widget tests require Qt Test, `dbus-run-session`, Python 3 with `dbus-python` and PyGObject:

```sh
meson setup build -Dtests=true
meson test -C build --print-errorlogs
```

Tests run on a private session bus with a mock daemon and never send commands to real hardware.
