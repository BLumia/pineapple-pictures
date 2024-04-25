Yet another image viewer.

|CI|Build Status|
|---|---|
|Windows Build|[![Windows build status](https://ci.appveyor.com/api/projects/status/dbd8clww3cit6oa0/branch/master?svg=true)](https://ci.appveyor.com/project/BLumia/pineapplepictures/branch/master)|
|macOS Build|[![macOS CI](https://github.com/BLumia/pineapple-pictures/actions/workflows/macos.yml/badge.svg)](https://github.com/BLumia/pineapple-pictures/actions/workflows/macos.yml)|
|Ubuntu Build|[![Ubuntu CI](https://github.com/BLumia/pineapple-pictures/actions/workflows/ubuntu.yml/badge.svg)](https://github.com/BLumia/pineapple-pictures/actions/workflows/ubuntu.yml)|

![Pineapple Pictures - Main Window](https://repository-images.githubusercontent.com/211888654/e8697600-e370-11eb-9b2a-b71e05262954)

## Summary

Pineapple Pictures is a lightweight image viewer that allows you view JPEG, PNG, GIF, SVG, PSD, KRA, XCF, TGA, HDR, AVIF and some other frequently used image formats files quickly and easily, and also provide a Stay-on-Top window setting that allows you pin the window so you can use it to pin a reference image at the top and then you can work with other software.

## Get it!

### Maintained by the original author

- [GitHub Release Page](https://github.com/BLumia/pineapple-pictures/releases)
- [SourceForge](https://sourceforge.net/projects/pineapple-pictures/)
- Archlinux AUR: [pineapple-pictures](https://aur.archlinux.org/packages/pineapple-pictures/) | [pineapple-pictures-git](https://aur.archlinux.org/packages/pineapple-pictures-git/)
- [Itch.io Store](https://blumia.itch.io/pineapple-pictures)

### Maintained by contributors / curtain distro's package maintainers

- Debian (since bullseye) or Ubuntu (since 21.04): `sudo apt install pineapple-pictures`
- Nix / NixOS: [pineapple-pictures](https://search.nixos.org/packages?channel=unstable&show=pineapple-pictures&from=0&size=50&sort=relevance&type=packages&query=pineapple-pictures) (maintained by @wineee)

## Help Translation!

[Translate this project on Weblate!](https://hosted.weblate.org/projects/pineapple-pictures/)

## Build it manually:

Current state, we need:

 - `cmake`: as the build system.
 - `qt5` with `qt5-svg` and `qt5-tools`: since the app is using Qt.
 - `libexiv2`: able to display more image metadata. (optional, but recommended)

Then we can build it with any proper c++ compiler like g++ or msvc.

Building it just requires normal cmake building steps:

``` bash
$ mkdir build && cd build
$ cmake ..
$ cmake --build . # or simply using `make` if you are using Makefile as the cmake generator.
```

After that, a `ppic` executable file will be available to use. You can also optionally install it by using the target `install` (or simply `make install` in case you are using Makefile). After the build process, you can also use `cpack` to make a package.

The project will try to build with `exiv2` when it's available at build time, if you would like to build the project without `exiv2`, pass `-DEXIV2_METADATA_SUPPORT=OFF` to `cmake`. The project will also not use `exiv2` if it's not found, the `EXIV2_METADATA_SUPPORT` option can be useful if you have `exiv2` but specifically don't want to use it.

Image formats supports rely on Qt's imageformats plugins, just get the plugins you need from your distro's package manager will be fine. For Windows user, you may need build and install the imageformats plugin manually, read the content below.

> [!NOTE]
> Although there is a `pineapple-pictures.pro` file which can be used for QMake build, it's only for testing purpose and it doesn't have `exiv2` support included. Using QMake to build this project is NOT supported, please use CMake if possible.

### Linux

Just normal build process as other program will be fine. Nothing special ;)

For Archlinux there are also a [PKGBUILD](https://aur.archlinux.org/cgit/aur.git/tree/PKGBUILD?h=pineapple-pictures-git) you can use.

For packaging to debian-based distro, the `CMakeLists.txt` provides some cpack configurations for generating a `.deb` package. After the build process, use `cpack -G DEB` to generate the package. You can also take `.github/workflows/ubuntu.yml` as a reference.

For this project, `DEB` is the only supported cpack generator in current state, feel free to submit a PR if you like improving `cpack` support for this project.

### Windows

The normal build steps for Linux is also applied to Windows, but since Windows doesn't have a decent package manager, so if you need any other image formats support other than the supported formats which Qt provided, you need to get and build these imageformats plugins manually and vendor it. It's optional and can be skipped if you don't need extra image formats support.

For the Windows binary I provided, kimageformats plugin is used (for formats like kra, xcf, psd and etc.). You can take `appveyor.yml` as a reference to learn what I did when building the Windows binary.

[KDE Craft](https://community.kde.org/Craft) environment also can be used to build and package this program. I did also created a blueprint for building this project that you can found it at [here](https://github.com/BearKidsTeam/craft-shmooprint-bkt). It's not the way I used to create the release binary, but still worth trying.

### macOS

I don't have a mac, so no support at all. There is also a GitHub Action (see `.github/workflows/macos.yml`) running macOS build though so at least it can build. Feel free to submit a PR if you would like to give some love to the macOS build ;P

## License

Pineapple Pictures as a whole is licensed under MIT license. Individual files may have a different, but compatible license.
