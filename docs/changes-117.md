# Changes with Anno 117

## Important

<div class="annotate" markdown>
-   [Changed Paths](#changed-paths) - Paths to `assets.xml` and others have changed. (1)
-   [Modinfo](./modinfo.md#mandatory-fields) - Only mods with game version `8` are loaded. (2)
-   [GUID Ranges](#safe-ranges) - No changes. (3)
-   [Mod Folders](#mod-folders) - No changes. (4)
</div>

1.  Most base game files are also under `data/base` now.

    E.g. `data/base/config/export/assets.xml`

2.  Only mods with game version `8` in `modinfo.json` are loaded.
    ```json
    {
      "Anno": 8,
      "Version": "1.0.0",
      "ModID": "first-mod-jakob"
      // ..
    }
    ```

3.  Reserve your GUID range.

    Open a PR on [GuidRanges](https://github.com/anno-mods/GuidRanges) or ask on [Discord](https://discord.gg/CUq2zQdV).

4.  The mod folders for manual installation are:

    - `<user>/Anno 117/mods/`
    - `<install>/Anno 117/mods/`

??? tip "Templates in Visual Studio Code"
    Press ++f1++ in [Anno Modding Tools](https://marketplace.visualstudio.com/items?itemName=JakobHarder.anno-modding-tools) and run `Anno: Create Mod from Template` to generate a template.

## Changes

- [Localization](#localization) - Texts use LineIDs now instead of GUIDs.
- [Modinfo](./modinfo.md#game-setup) - New `Difficulty` entry is mandatory, but mods load without it.
- [InfoTips](./infotips.md#structure) - The format has changed for better readability.
- [Dependencies](./modinfo.md#dependencies) - Changed dependency entries in modinfo.

## New Features

- [Short ModOps](./new-features-117.md#short-modops) - Shorter ModOps.
- [Directly Add Assets](./new-features-117.md#add-assets) - Conveniently add assets. Handles BaseAssetGUID automatically.
- [Mod Profiles](./profiles.md) - The game loads mod activations from `<user>/Anno 117/active-profile.txt`.
- [ModOps Property](./new-features-117.md#property-lookup) - Property Lookup
- [ModOps MaxRepeat](./new-features-117.md#property-lookup) - Use `Group` with `Condition` for loops.
- [InfoTips](./infotips.md#select-infotip) - GUID Lookup
- [XPath Functions](./new-features-117.md#xpath) - XPath functions like `number()` and `count()` can be used.
- [ModID as XPath Variable](./modops/control.md#modid-condition) - Do things like `#mod-a and not(#mod-b)`.
- [Inline ModOps](./modops/content.md) - Merge enums, insert local content and other `Content` improvements.
- [ModItem](./modops/lists.md) - More control over merging items and lists with items.
- [Lua Scripts](./lua-scripts.md) - Script support

## Extra Notes

### Changed Paths

The location of the main `assets.xml` has changed.

=== ":material-pillar: 117"
    `data/base/config/export/assets.xml`
=== ":material-factory: 1800"
    `data/config/export/main/asset/assets.xml`

Most base game files are also under `data/base` now.

!!! tip "Templates in Visual Studio Code"
    Press ++f1++ in [Anno Modding Tools](https://marketplace.visualstudio.com/items?itemName=JakobHarder.anno-modding-tools) and run `Anno: Create Mod from Template` to generate a template.

### Mod Folders

The locations of the mod folders for manual installation are unchanged.

- `<user>/Anno 117/mods/`
- `<install>/Anno 117/mods/`

### Safe Ranges

!!! info inline end "Reserve your range"
    Open a PR on [GuidRanges](https://github.com/anno-mods/GuidRanges) or ask on [Discord](https://discord.gg/CUq2zQdV).

The GUID ranges safe to use for mods have not changed.

Name | Range
-- | --
GUIDs         | 1.337.471.142 - 2.147.483.647
LineIDs       | 1.337.471.142 - 2.147.483.647
Personal use  | 2.001.000.000 - 2.001.009.999
Enums         | will come with future releases

### Localization

Text IDs are based on Oasis Line IDs now instead of `GUID`s.

Vanilla text IDs are always negative numbers like `-6911555031864321364` (or `A015 3951 4829 DAAC` in hex).

For modding purposes it is best to simply use the same range as GUIDs.
E.g. matching LineID of a building name with the GUID of the building asset.

=== ":material-pillar: 117"
    ```xml
    <ModOp Add="/TextExport/Texts[1]">
      <Text>
        <Text>Happy Modding</Text>
        <LineId>2001000000</LineId>
      </Text>
    </ModOp>
    ```

    ```xml
    <Values>
      <!-- ... -->
      <Text>
        <OasisId>2001000000</OasisId>
      </Text>
    </Values>
    ```

=== ":material-factory: 1800"
    ```xml
    <ModOp Add="/TextExport/Texts[1]">
      <Text>
        <Text>Happy Modding</Text>
        <GUID>2001000000</GUID>
      </Text>
    </ModOp>
    ```

    ```xml
    <Values>
      <!-- ... -->
      <Standard>
        <GUID>2001000000</GUID>
      </Standard>
    </Values>
    ```

