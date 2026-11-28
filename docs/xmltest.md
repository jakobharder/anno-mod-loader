# Test Patches with xmltest

{{all}}

Use xmltest to test patches against extracted game files without starting the game.
Download and extract `xmltest.zip` from the [releases page](https://github.com/jakobharder/anno-mod-loader/releases/latest).

Executable | Game
--- | ---
`xmltest117.exe` | Anno 117
`xmltest1800.exe` | Anno 1800

## Patch a File

Run the tool with the original game file and your mod's patch.
Use `-m` to specify the mod root so [includes](./modops/control.md#include) resolve correctly.

These examples assume the tools, an extracted `assets.xml`, and the `my-mod` folder are in the current directory.

=== ":material-pillar: 117"
    ```powershell
    .\xmltest117.exe -m my-mod -o patched.xml assets.xml my-mod\data\base\config\export\assets.xml
    ```
=== ":material-factory: 1800"
    ```powershell
    .\xmltest1800.exe -m my-mod -o patched.xml assets.xml my-mod\data\config\export\main\asset\assets.xml
    ```

The result is written to `patched.xml`. Add `-v` for detailed diagnostics,
or use `-s` instead of `-o` to check the patch without writing a file.

You can also test [CFG/FC patches](./cfg-fc-patches.md) and [InfoTip patches](./infotips.md).
Use the original game file as the target and its `.xml` sidecar as the patch.
Choose an `.xml` output filename to inspect binary files as XML.

## Show an Asset

Print an asset by GUID:

```powershell
.\xmltest117.exe -c show assets.xml 1010372
```

## Compare Changes

```powershell
.\xmltest117.exe -c diff -m my-mod assets.xml my-mod\data\base\config\export\assets.xml
```

This prints patched and original XML for changed assets.
For a visual comparison, use [Anno Modding Tools](https://anno-mods.github.io/vscode-anno/editing/analysis/).

## Options

The examples use `xmltest117.exe`; use `xmltest1800.exe` and the corresponding
[file paths](./file-structure.md) for Anno 1800.
Pass option values as separate arguments, e.g. `-c diff`.

Option | Description
--- | ---
`-c command` | `patch` (default), `show`, or `diff`.
`-m path` | Mod root. Defaults to the current directory. Repeat for additional include search roots.
`-p path` | Apply another mod's patch first. Repeat in loading order; dependencies are not discovered automatically.
`-o path` | Output file. Defaults to `patched` plus the input extension. Used by `patch` only.
`-s` | Skip writing the patched file. Cannot be combined with `-o`.
`-v` | Verbose diagnostics.
`-i relative-path` | Read patch content from standard input for this mod-relative file. Target and patch arguments are still required.

Run either tool without arguments to print usage.
Check diagnostics and the result alongside the game's [mod-loader.log](./debug.md).

Note: Game options and active-mod variables are not loaded automatically, so `$option` and `#mod-id` conditions may differ from the game.
