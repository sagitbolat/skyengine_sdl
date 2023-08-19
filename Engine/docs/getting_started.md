# Getting Started with the SkyEngine

## Table of contents

1. [Installation](#1-installation)
    - [System Requirements](#11-system-requirements)
    - [Installation Steps](#12-installation-steps)
2. [Your First Project](#2-your-first-project)
    - [Creating your projec](#21-creating-your-project)
    - [Building your project](#22-building-your-project)
3. [Basic Usage](#3-basic-usage)

## 1. Installation

In this section, you will learn how to install the SkyEngine on your local system.

### 1.1 System Requirements

The engine will run on most operating systems, but has a set of dependencies:

- Your system needs to be compatible with **OpenGL 3.3** or later. 
- You need to have **SDL2** installed.
- You need to have **OpenAL** installed.

As of version 1.0 (Aug 19th 2023), only the Windows and Linux have working build systems. MacOSX users can write their own build system by recycling the **build.sh** file if they wish.

### 1.2 Installation Steps
These steps are the same for all operating systems.

1. Navigate to [the SkyEngine github repository](https://github.com/SkyGameEngine/engine).
2. Either `clone` the repository or download the source code as a zip.
3. The project has a number of files and directories. All users require the `/Engine` directory, as that is the source code of the engine as well as the `create_project.py` file. For Windows users, you also need the `build.bat` file, and Linux users will need the `build.sh` file. It is wise to also keep the `debug.bat` or `debug.sh` files, as these will allow you to run your projects with `gdb` enabled.
4. You can delete all files not mentioned in the previous step.

## 2. Your First Project

This section describes how project creation and project building processes look like in the SkyEngine.

### 2.1 Creating your project

After the installation is complete, run 
```
python create_project.py [project_name]
``` 
in the same directory as your build scripts are located in. This should generate a new directory with your chosen `[project_name]`.

The layout of your directories should now be:

- Root directory
    - Engine directory
    - [project_name] directory
    - Build and Debug scripts

Inside your project directory, you will find a `main.cpp` file. You may change the name of the file if you wish.

### 2.2 Building your project

To build your project, simply run 
```
build.bat [project_name] main
``` 
from the root directory (same directory as the build script). Replace `build.bat` with `build.sh` for Linux. If you changed the name of `main.cpp` in the previous step, change the `main` in the command to that new name. Ex: if your project directory is called `my_project`, and you renamed `main.cpp` to `my_file.cpp`, you would run 
```
build.bat my_project my_file
```

## 3. Basic Usage



