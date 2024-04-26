# Anno 1800 Mod Loader

> [!NOTE]
> This is a fork of the original mod loader from the great [meow](https://github.com/xforce/anno1800-mod-loader).
> 
> Many thanks to him for creating and maintaining that project for so long.

Latest Changes:
 - [Modloader changes with GU17](./doc/modloader10.md)
 - [Modloader changes with GU18](./doc/modloader11.md)

You can find beginner friendly tutorials in the community [modding guide](https://github.com/anno-mods/modding-guide#readme).

## Install and Activate Mods

### Mod Loader

The game includes the modloader as of GU17 it.
You do not need to install the mod loader.

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

## Asset Modding

In previous anno games there was a way to tell the game to load extacted files from disk instead of loading them  
from the RDA container. While that made it easier, it's still not a nice way to handle modding large XML files.

This Anno 1800 mod loader supports a few simple 'commands' to easily patch the XML to achieve pretty much whatever you want.

### How to Create a Patch for any XML File from the Game:

**Step 1)** Set up a directory for your mod inside Anno 1800/mods. In the following steps, it is assumed that you have titled your directory "myMod"

**Step 2)** inside of myMod, you recreate the exact file structure that the base game uses. A patched assets.xml file would have to be under the following path: `Anno 1800/mods/myMod/data/config/export/main/asset/assets.xml`

**Step 3)** Your XML document is expected to have the following structure:

```xml
<ModOps>
    <ModOp>
        <!-- Whatever Change you want to do -->
    </ModOp>
</ModOps>
```

> You can give as many `<ModOp>` as you'd like to and have multiple patch files for different original ones in a single mod.

### How to Write a ModOp

**Step 1)** Look up and select the XML node you want to edit with XPath using the Path argument.

Example:

```xml
<ModOp Path = "/Templates/Group[Name = 'Objects']/Template[Name = 'Residence7']/Properties">
```

For the assets file, you can also use the GUID argument. This selects all the child nodes of the asset with the given GUID as new roots for your xPath for cleaner code and is also much faster, performance-wise.

Example:

```xml
    Standard way:               <ModOp Path = "//Asset[Values/Standard/GUID = '1137']/Values/Standard/Name">

    Better, with GUID arg:      <ModOp GUID = '1337' Path = "/Values/Standard/Name">
```

**Step 2)** Give a type for a ModOp, to change the selected node.

Currently supported types:

```
- Merge                 Replaces all given child nodes or Arguments
- Remove                Removes the selected Node
- Add                   Adds inside the selected Node
- Replace               Replaces the selected Node
- AddNextSibling        Adds a sibling directly after the selected node
- AddPrevSibling        Adds a sibling directly in front of the selected node
```

> This was just a quick initial implementation (~3h), very open for discussions on how to make that better or do something entirely different

Detailed type documentation:
- [Add, remove, addNextSibling, addPrevSibling](./doc/modop-guide.md)
- [Replace and merge](./doc/modop-replace-merge.md)
- [Conditions](./doc/modop-conditions.md)
- [Examples](./doc/modop-examples.md)

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
