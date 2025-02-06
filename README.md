# OpenAnimationReplacer-DetectionPlugin

This is a SKSE64 plugin adding detection condition to Open Animation Replacer using its API.

### Requirements

-   [XMake](https://xmake.io) [2.8.2+]
-   C++23 Compiler (MSVC, Clang-CL)

## Getting Started

```bat
git clone --recurse-submodules git@github.com:matiasmakipelto/OpenAnimationReplacer-DetectionConditions.git
cd OpenAnimationReplacer-DetectionConditions
```

### Build

To build the project, run the following command:

```bat
xmake build
```

> **_Note:_** _This will generate a `build/windows/` directory in the **project's root directory** with the build output._

### Build Output

Change the folders where to copy the build from xmake.lua file:

### Project Generation (Optional)

If you want to generate a Visual Studio project, run the following command:

```bat
xmake project -k vsxmake
```

> **_Note:_** _This will generate a `vsxmakeXXXX/` directory in the **project's root directory** using the latest version of Visual Studio installed on the system._

### Upgrading Packages (Optional)

If you want to upgrade the project's dependencies, run the following commands:

```bat
xmake repo --update
xmake require --upgrade
```
