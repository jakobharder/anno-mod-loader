# Anno Mod Loader

> [!IMPORTANT]
> [Documentation](https://jakobharder.github.io/anno-mod-loader)
>
> You can find beginner friendly tutorials in the community [modding guide](https://github.com/anno-mods/modding-guide#readme).

This is a fork of the original mod loader of the great [meow](https://github.com/xforce/anno1800-mod-loader).
Many thanks to him for creating and maintaining that project for so long.

## Install and Activate Mods

### Mods

There are two locations for mods:
- `This PC\Documents\Anno 1800\mods`
- In `mods/` within your game installation,
  typically `C:\Program Files (x86)\Ubisoft\Ubisoft Game Launcher\games\Anno 1800\mods`

The order of loaded mods is decided by `LoadAfterIds` in `modinfo.json`.

If a mod is found twice, the newest version will be used.

### Activation

You can deactivate mods by adding `-` or `.` in front of the folder name.

Additional, mods can be disabled with a `activations.json` in the respective `mods/` folders:

```json
{
  "disabledIds": [
    "ModA",
    "ModB"
  ]
}
```

`ModID` from `modinfo.json` is used, and if not specified the folder name of the mod as a fallback.

### Split XML Patch into Multiple Files

You can split your XML patches into multiple files by using `Include` instructions.

```xml
<ModOps>
    <!-- ModOps applied before the include -->
    <Include File="even-more-modops.include.xml" />
    <!-- ModOps applied after the include -->
</ModOps>
```

`File` takes a file path relative to the XML file that does the include.

XML files without a counterpart in the game are normally mistakes and lead to errors in the log.
Use the extension `*.include.xml` to prevent that.

Otherwise, included XML patches are handled the same way as normal XML patches. Nesting includes is supported.

You can find more help and examples in the community [modding guide](https://github.com/anno-mods/modding-guide#readme).

## Trouble Shooting

The game writes a new log file every game start `<User>/Documents/Anno 1800/log/mod-loader.log`. You can find issues and mod loading order there.

To test what a 'patch' you write does to the original game file, you can also use `xmltest.exe`. It will simulate what the game will load.

```
xmltest.exe c:\anno\all-rda\assets.xml patch.xml
```

Hint: You can use a plugin called [Anno Modding Tools](https://marketplace.visualstudio.com/items?itemName=JakobHarder.anno-modding-tools) for Visual Studio Code for more [powerful patch testing](https://marketplace.visualstudio.com/items?itemName=JakobHarder.anno-modding-tools#command-compare).

## For Developers

You need Bazel, Visual Studio 2022 and that _should_ be it.
You can checkout `azure-pipelines.yml` and see how it's done there.

easy steps to sucess:

- Install Visual Studio 2022 (community version is fine + C++ tools)
- Install Bazel (I recommend going with Bazelisk here)
- _optional_ fork this repo
- clone this repo and its submodules:
  `git submodule update --init --recursive`
- open the folder in Visual Studio 2022
- _optional_ make changes
- open a command prompt (admin) & navigate to the workingdir
- If you have installed another version of Visual Studio as well:
  - `set BAZEL_VC=C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC`
  - this will use the correct build tools (if your VS 2022 install dir differ, please adapt)
- use `bazel build //libs/python35:python35.dll` to build the .dll
- find the DLL in your workingdir \bazel-bin\libs\python35

If you want to work on new features for XML operations, you can use xmltest for testing. As that is using the same code as the actualy file loader.
