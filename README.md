Yet another image viewer.

|CI|Build Status|
|---|---|
|Windows Build|[![Windows build status](https://ci.appveyor.com/api/projects/status/dbd8clww3cit6oa0/branch/master?svg=true)](https://ci.appveyor.com/project/BLumia/pineapplepictures/branch/master)|
|macOS Build|![macOS CI](https://github.com/BLumia/pineapple-pictures/workflows/macOS%20CI/badge.svg)|
|Ubuntu 20.04 Build|![Ubuntu 20.04 CI](https://github.com/BLumia/pineapple-pictures/workflows/Ubuntu%2020.04%20CI/badge.svg)|

![Pineapple Pictures - Main Window](https://repository-images.githubusercontent.com/211888654/21fb6300-269f-11ea-8e85-953e5d57da44)

## Get it!

 - [GitHub Release Page](https://github.com/BLumia/pineapple-pictures/releases)
 - Archlinux AUR: [pineapple-pictures-git](https://aur.archlinux.org/packages/pineapple-pictures-git/)

## Build it manually:

Current state, we need:

 - `cmake`: as the build system.
 - `qt5` with `qt5-svg` and `qt5-tools`: since the app is using Qt.

Then we can build it with any proper c++ compiler like g++ or msvc.

Building it just requires normal cmake building steps:

``` bash
$ mkdir build && cd build
$ cmake ..
$ cmake --build . # or simply using `make` if you are using Makefile as the cmake generator.
```

After that, a `ppic` executable file will be available to use. You can also optionally install it by using the target `install` (or simply `make install` in case you are using Makefile). After the build process, you can also use `cpack` to make a package.

Image formats supports rely on Qt's imageformats plugins, just get the plugins you need from your distro's package manager will be fine. For Windows user, you may need build and install the imageformats plugin manually, read the content below.

### Linux

Just normal build process as other program will be fine. Nothing special ;)

For Archlinux there are also a [PKGBUILD](https://aur.archlinux.org/cgit/aur.git/tree/PKGBUILD?h=pineapple-pictures-git) you can use.

For packaging to debian-based distro, the `CMakeLists.txt` provides some cpack configurations for generating a `.deb` package. After the build process, use `cpack -G DEB` to generate the package. You can also take `.github/workflows/ubuntu.yml` as a reference.

For this project, `DEB` is the only supported cpack generator in current state, feel free to submit a PR if you like improving `cpack` support for this project.

### Windows

The normal build steps for Linux is also applied to Windows, but since Windows doesn't have a decent package manager, so if you need any other image formats support other than the supported formats which Qt provided, you need to get and build these imageformats plugins manually and vendor it. It's optional and can be skipped if you don't need extra image formats support.

For the Windows binary I provided, kimageformats plugin is used (for formats like kra, xcf, psd and etc.). You can take `appveyor.yml` as a reference to learn what I did when building the Windows binary.

[KDE Craft](https://community.kde.org/Craft) environment also can be used to build and package this program. I did also created a blueprint for building this project, but since I don't have a CI to run KDE Craft build, the blueprint repo are not provided here. Maybe sometimes later.

### macOS

I don't have a mac, so no support at all. There is also a GitHub Action (see `.github/workflows/macos.yml`) running macOS build though so at least it can build. Feel free to submit a PR if you would like to give some love to the macOS build ;P

## Help Translation!

[Translate this project on Transifex!](https://www.transifex.com/blumia/pineapple-pictures/)

Feel free to open up an issue to request a new language to translate.

## License

This program released under MIT license
