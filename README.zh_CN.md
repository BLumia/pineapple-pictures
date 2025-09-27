简单轻量的跨平台看图工具。

|CI|构建状态|
|---|---|
|Windows Build|[![Windows CI](https://github.com/BLumia/pineapple-pictures/actions/workflows/windows.yml/badge.svg)](https://github.com/BLumia/pineapple-pictures/actions/workflows/windows.yml)|
|macOS Build|[![macOS CI](https://github.com/BLumia/pineapple-pictures/actions/workflows/macos.yml/badge.svg)](https://github.com/BLumia/pineapple-pictures/actions/workflows/macos.yml)|
|Ubuntu Build|[![Ubuntu CI](https://github.com/BLumia/pineapple-pictures/actions/workflows/ubuntu.yml/badge.svg)](https://github.com/BLumia/pineapple-pictures/actions/workflows/ubuntu.yml)|

![Pineapple Pictures - Main Window](https://repository-images.githubusercontent.com/211888654/e8697600-e370-11eb-9b2a-b71e05262954)

## 简介

菠萝看图是一个轻量图像查看器，允许你简单快捷的查看 JPEG, PNG, GIF, SVG, PSD, KRA, XCF, TGA, HDR, AVIF 等常用格式的图像文件，并提供了置顶窗口的选项以便你在使用其它软件时也可以将参考图片固定在顶端。

## 立即获取!

### 由原作者维护

- [GitHub Release 页面](https://github.com/BLumia/pineapple-pictures/releases)
- [SourceForge](https://sourceforge.net/projects/pineapple-pictures/)
- Archlinux AUR: [pineapple-pictures](https://aur.archlinux.org/packages/pineapple-pictures/) | [pineapple-pictures-git](https://aur.archlinux.org/packages/pineapple-pictures-git/)
- [Itch.io 商店](https://blumia.itch.io/pineapple-pictures)
- Flatpak (于 FlatHub): [net.blumia.pineapple-pictures](https://flathub.org/apps/net.blumia.pineapple-pictures) *([我应当使用 flatpak 版吗？](https://github.com/BLumia/pineapple-pictures/wiki/Container%E2%80%90based-Packaging-Solutions-Support))*

### 由贡献者/对应发行版的打包人员维护

[![打包状态](https://repology.org/badge/vertical-allrepos/pineapple-pictures.svg?columns=4)](https://repology.org/project/pineapple-pictures/versions)

## 参与贡献

除了参与反馈和代码贡献外，也欢迎各种类型的贡献！

### 帮助翻译!

[在 Weblate 上帮助此项目翻译到更多语言!](https://hosted.weblate.org/projects/pineapple-pictures/)

### 打赏

[![Afdian](https://static.afdiancdn.com/static/img/logo/logo.png)Afdian | 爱发电](https://afdian.com/a/BLumia)

[![ko-fi](https://ko-fi.com/img/githubbutton_sm.svg)](https://ko-fi.com/blumia)

## 手动构建步骤:

当前状态，我们需要先确保如下依赖可用:

 - `cmake`: 我们所使用的构建系统
 - 包含 `qt6-svg` 与 `qt6-tools` 组件的 `qt6`: 此应用基于 Qt
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

在 Windows、Linux 以及 macOS 系统均可构建此应用，其它有移植 Qt 支持的平台也可能可以进行构建。若要了解一些平台相关的构建指引，请参阅[相关的 Wiki 页面](https://github.com/BLumia/pineapple-pictures/wiki/Platform-Specific-Build-Instructions)。

> [!NOTE]
> 尽管存在一个可用于 QMake 构建的 `pineapple-pictures.pro` 文件，但其仅供简单测试所用且其并不包含 `exiv2` 支持。使用 QMake 构建此项目是 **不受支持** 的，请尽可能考虑使用 CMake。

## 许可协议

菠萝看图整体使用 MIT 协议进行发布。项目所随的部分源文件可能具备不同但与之兼容的许可协议。
