Yet another image viewer.

|CI|Build Status|
|---|---|
|Windows Build|[![Windows CI](https://github.com/BLumia/pineapple-pictures/actions/workflows/windows.yml/badge.svg)](https://github.com/BLumia/pineapple-pictures/actions/workflows/windows.yml)|
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

### Maintained by contributors / certain distro's package maintainers

[![Packaging status](https://repology.org/badge/vertical-allrepos/pineapple-pictures.svg?columns=4)](https://repology.org/project/pineapple-pictures/versions)

## Contribution

Beside feedback and code contribution, other contributions are also welcome!

### Help Translation!

[Translate this project on Weblate!](https://hosted.weblate.org/projects/pineapple-pictures/)

### Funding

[![ko-fi](https://ko-fi.com/img/githubbutton_sm.svg)](https://ko-fi.com/blumia)

[![Afdian](https://static.afdiancdn.com/static/img/logo/logo.png)Afdian](https://afdian.com/a/BLumia)

## Build it manually:

Current state, we need:

 - `cmake`: as the build system.
 - `qt6` with `qt6-svg` and `qt6-tools`: since the app is using Qt.
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

It's possible to build it under Windows, Linux, macOS, and maybe other desktop platforms that Qt is ported to. For platform specific build instructions, please read the [related wiki page](https://github.com/BLumia/pineapple-pictures/wiki/Platform-Specific-Build-Instructions).

> [!NOTE]
> Although there is a `pineapple-pictures.pro` file which can be used for QMake build, it's only for testing purpose and it doesn't have `exiv2` support included. Using QMake to build this project is NOT supported, please use CMake if possible.

## License

Pineapple Pictures as a whole is licensed under MIT license. Individual files may have a different, but compatible license.
