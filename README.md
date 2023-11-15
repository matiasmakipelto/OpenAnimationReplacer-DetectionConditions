# OpenAnimationReplacer-ExamplePlugin

This is a SKSE64 plugin example utilizing Open Animation Replacer's API to add a new custom condition.

### Requirements
* [XMake](https://xmake.io) [2.7.8+]
* C++20 Compiler (MSVC, Clang?)

## Getting Started
```bat
git clone git@github.com:matiasmakipelto/OpenAnimationReplacer-DetectionConditions.git
cd OpenAnimationReplacer-DetectionConditions
```

### Build
To build the project, run the following command:
```bat
xmake build
```

> ***Note:*** *This will generate a `build/windows/` directory in the **project's root directory** with the build output.*

### Build Output
Change the folders where to copy the build from xmake.lua file:

### Project Generation (Optional)
If you want to generate a Visual Studio project, run the following command:
```bat
xmake project -k vsxmake
```

> ***Note:*** *This will generate a `vsxmakeXXXX/` directory in the **project's root directory** using the latest version of Visual Studio installed on the system.*

### Upgrading Packages (Optional)
If you want to upgrade the project's dependencies, run the following commands:
```bat
xmake repo --update
xmake require --upgrade
```
