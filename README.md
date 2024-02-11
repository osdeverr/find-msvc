# find-msvc - the better `vswhere`

**find-msvc** is a single-file, dependency-free executable that finds the latest installed Visual C++ development tools and the Windows SDK.

Unlike vswhere, find-msvc finds not just the Visual Studio installation but all the paths, versions and envrionment variables required to run the C++ build toolchain.

This is helpful to set up a clean C++ work environment in scripts without invoking the clunky Visual Studio Developer Command Prompt with its .bat files and whatnot.

find-msvc is used by and built with [Re](https://github.com/osdeverr/rebs) - *the* build system for C++.

## What does it do?
find-msvc outputs a single JSON object with info about the latest MSVC toolchain.

### Example Output
```json
> ./find-msvc.exe
{
    "environment": {
        "INCLUDE": "C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.38.33130/include;C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.38.33130/ATLMFC/include;C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Auxiliary/VS/include;C:/Program Files (x86)/Windows Kits/10/include/10.0.22621.0/cppwinrt;C:/Program Files (x86)/Windows Kits/10/include/10.0.22621.0/shared;C:/Program Files (x86)/Windows Kits/10/include/10.0.22621.0/ucrt;C:/Program Files (x86)/Windows Kits/10/include/10.0.22621.0/um;C:/Program Files (x86)/Windows Kits/10/include/10.0.22621.0/winrt",
        "VCToolsInstallDir": "C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.38.33130",
        "VCToolsVersion": "14.38.33130",
        "WindowsSDKLibVersion": "10.0.22621.0",
        "WindowsSDKVersion": "10.0.22621.0",
        "WindowsSdkBinPath": "C:/Program Files (x86)/Windows Kits/10/bin",
        "WindowsSdkDir": "C:/Program Files (x86)/Windows Kits/10/",
        "WindowsSdkVerBinPath": "C:/Program Files (x86)/Windows Kits/10/bin/10.0.22621.0"
    },
    "vc_include_dirs": [
        "C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.38.33130/include",
        "C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.38.33130/ATLMFC/include",
        "C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Auxiliary/VS/include",
        "C:/Program Files (x86)/Windows Kits/10/include/10.0.22621.0/cppwinrt",
        "C:/Program Files (x86)/Windows Kits/10/include/10.0.22621.0/shared",
        "C:/Program Files (x86)/Windows Kits/10/include/10.0.22621.0/ucrt",
        "C:/Program Files (x86)/Windows Kits/10/include/10.0.22621.0/um",
        "C:/Program Files (x86)/Windows Kits/10/include/10.0.22621.0/winrt"
    ],
    "vc_tools_dir": "C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.38.33130",
    "vc_tools_version": "14.38.33130",
    "vs_install_dir": "C:/Program Files/Microsoft Visual Studio/2022/Community",
    "winsdk_bin": "C:/Program Files (x86)/Windows Kits/10/bin/10.0.22621.0",
    "winsdk_directory": "C:/Program Files (x86)/Windows Kits/10/",
    "winsdk_version": "10.0.22621.0"
}
```
