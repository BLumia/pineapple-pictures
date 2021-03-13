简单轻量的跨平台看图工具。

|CI|构建状态|
|---|---|
|Windows Build|[![Windows build status](https://ci.appveyor.com/api/projects/status/dbd8clww3cit6oa0/branch/master?svg=true)](https://ci.appveyor.com/project/BLumia/pineapplepictures/branch/master)|
|macOS Build|![macOS CI](https://github.com/BLumia/pineapple-pictures/workflows/macOS%20CI/badge.svg)|
|Ubuntu 20.04 Build|![Ubuntu 20.04 CI](https://github.com/BLumia/pineapple-pictures/workflows/Ubuntu%2020.04%20CI/badge.svg)|

![Pineapple Pictures - Main Window](https://repository-images.githubusercontent.com/211888654/21fb6300-269f-11ea-8e85-953e5d57da44)

## 简介

菠萝看图是一个轻量图像查看器，允许你简单快捷的查看 JPEG, PNG, GIF, SVG, PSD, KRA, XCF, TGA, AVIF 等常用格式的图像文件，并提供了置顶窗口的选项以便你在使用其它软件时也可以将参考图片固定在顶端。

## 立即获取!

 - [GitHub Release 页面](https://github.com/BLumia/pineapple-pictures/releases) | [gitee 发布页面](https://gitee.com/blumia/pineapple-pictures/releases)
 - [SourceForge](https://sourceforge.net/projects/pineapple-pictures/)
 - Archlinux AUR: [pineapple-pictures](https://aur.archlinux.org/packages/pineapple-pictures/) | [pineapple-pictures-git](https://aur.archlinux.org/packages/pineapple-pictures-git/)
 - Debian (sid / testing): `sudo apt install pineapple-pictures`
 - [Itch.io 商店](https://blumia.itch.io/pineapple-pictures)

## 帮助翻译!

[在 Weblate 上帮助此项目翻译到更多语言!](https://hosted.weblate.org/projects/pineapple-pictures/)

## 手动构建步骤:

当前状态，我们需要先确保如下依赖可用:

 - `cmake`: 我们所使用的构建系统
 - 包含 `qt5-svg` 与 `qt5-tools` 组件的 `qt5`: 此应用基于 Qt
 - `libexiv2`: 用以获取和显示更多的图像元信息（可选，推荐）

然后我们就可以使用任何常规的 c++ 编译器如 g++ 或 msvc 来进行构建了

构建过程就是常规的 CMake 应用构建过程:

``` bash
$ mkdir build && cd build
$ cmake ..
$ cmake --build . # 如果你使用 Makefile 作为 CMake 生成器，也可以直接简单的使用 `make`
```

完毕后，一个名为 `ppic` 的可执行程序即会被生成以供使用。您也可以选择通过使用 CMake 生成的 `install` 目标继续将其安装到您的设备上（假设您使用 Makefile，即可执行 `make install` 来进行安装）。构建步骤完毕后，您也可以使用 `cpack` 来对应用程序进行打包。

当 `exiv2` 在构建时可用时，此项目将尝试使用其进行构建，若您不希望使用 `exiv2`，请传递 `-DEXIV2_METADATA_SUPPORT=OFF` 参数给 `cmake`。此项目在找不到 `exiv2` 时并不会使用 `exiv2`，`EXIV2_METADATA_SUPPORT` 选项可供尽管存在可用的 `exiv2` 但您明确不希望启用其支持时使用。

此应用的图片格式支持依赖于 Qt 的 imageformats 插件，直接从您所用的发行版获取对应的图像格式插件即可。对于 Windows 用户，您可能需要手动构建和使用图像格式插件。下方给出了进一步的说明。

### Linux

常规的构建步骤即可完成构建，不需要额外的处理步骤 ;)

对于 Archlinux 发行版的用户，这里还有一个 [PKGBUILD](https://aur.archlinux.org/cgit/aur.git/tree/PKGBUILD?h=pineapple-pictures-git) 可供使用和参考。

对于在基于 debian 的发行版中进行打包的需求， `CMakeLists.txt` 已经提供了一些基本的 cpack 配置以便生成一个有效的 `.deb` 软件包。在构建步骤完毕后，使用 `cpack -G DEB` 即可生成 DEB 软件包。您也可以参考 `.github/workflows/ubuntu.yml` 来查看当前正在使用的 CI 配置是如何进行打包的。

目前，`DEB` 是当前唯一受到直接支持的 cpack 生成目标。若希望为此项目添加其它的 cpack 目标支持，欢迎发起合并请求。

### Windows

上述的构建步骤在 Windows 中也适用，但由于 Windows 中不具备类如大多 Linux 发行版中所提供的方便的软件包管理机制，故如果您需要任何 Qt 官方支持之外的图像格式例如 psd，xcf，kra 等格式的支持，你就可能需要自行获取并构建对应的 imageformats 插件，并在您最终生成的可执行文件中一并提供这些插件。若您不需要这些额外的图像格式支持，这个步骤也可以直接跳过。

我们所提供的预编译好的 Windows 程序包含了 kimageformats 插件来提供额外（kra, xcf, psd 等）格式的支持。您可以参考 `appveyor.yml` 来查看我们是如何构建并打包 Windows 可执行程序的。

[KDE Craft](https://community.kde.org/Craft) 环境也可以被用来构建此应用程序。我也创建了一个蓝图来进行此项目的构建和打包。但由于暂时并未配置 CI 部署此环境来进行 KDE Craft 环境下的构建，故对应的蓝图仓库也尚未公开提供，或许后续会开放出来。

### macOS

由于我没有 mac 设备，故 macOS 暂时不受任何支持。不过我们目前有一个 GitHub Action 来执行 macOS 环境下的构建（见 `.github/workflows/macos.yml`）所以至少 macOS 下是可以顺利进行构建的。如果您想完善对 macOS 的支持，也欢迎您创建合并请求 ;P

## 许可协议

菠萝看图整体使用 MIT 协议进行发布。项目所随的部分源文件可能具备不同但与之兼容的许可协议。
