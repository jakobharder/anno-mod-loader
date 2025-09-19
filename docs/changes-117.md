# Changes with Anno 117

## Important

<div class="annotate" markdown>
-   [Changed Paths](./file-structure.md#basic-file-structure) - Paths to `assets.xml` and others have changed. (1)
-   [Modinfo](./modinfo.md#mandatory-fields) - Only mods with game version `8` are loaded. (2)
-   No changes to [GUID Ranges](#safe-ranges) (3) or [Mod Folders](./install.md#mod-folders). (4)
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

<div class="annotate" markdown>

- [Short ModOps](./modops/index.md#choose-type) (1)
- [Asset](./modops/basics.md#asset) - Add without ModOp (2)
- [Mod Profiles](./profiles.md) (3)
- [Property Lookup](./modops/index.md#lookup-helper) (4)
- [Loop Groups with MaxRepeat](./modops/control.md#loop-condition) - Use `Group` with `Condition` for loops.
- [InfoTips](./infotips.md#select-infotip) - GUID Lookup
- [XPath Functions](./new-features-117.md#xpath) - XPath functions like `number()` and `count()` can be used.
- [ModID as XPath Variable](./modops/control.md#modid-condition) - Do things like `#mod-a and not(#mod-b)`.
- [Inline ModOps](./modops/content.md) - Merge enums, insert local content and other `Content` improvements.
- [ModItem](./modops/lists.md) - More control over merging items and lists with items.
- [Lua Scripts](./lua-scripts.md) - Script support

</div>

1.  ModOps can be shortened with `Merge=<XPath>` instead of the old long form `Type="merge" Path=<XPath>`.

    === ":material-pillar: Short"
        ```xml
        <ModOp GUID="1010372"
               Merge="Building">
          <AllowChangeVariation>1</AllowChangeVariation>
        </ModOp>
        ```

    === ":material-animation-outline: Legacy"
        ```xml
        <ModOp GUID="1010372"
               Type="merge" Path="/Values/Building">
          <AllowChangeVariation>1</AllowChangeVariation>
        </ModOp>
        ```

2.  {{a117r}}

    Directly drop assets into your file without `ModOp`.
    The order of `BaseAssetGUID` is automatically handled.

    === ":material-pillar: 117"
        ```xml
        <ModOps>
          <Asset>
            <BaseAssetGUID>100780</BaseAssetGUID>
            <Values> <!-- .. --> </Values>
          </Asset>
        </ModOps>
        ```
    === ":material-animation-outline: 117 & 1800 ⚠️"
        ```xml
        <ModOps>
          <ModOp Type="addNextSibling" GUID="100780">
            <Asset>
              <BaseAssetGUID>100780</BaseAssetGUID>
              <Values> <!-- .. --> </Values>
            </Asset>
          </ModOp>
        </ModOps>
        ```

3.  The game loads mod activations from a text file at `<user>/Anno 117/active-profile.txt`.

4.  New lookup helper to select properties.

    === ":material-pillar: 117"
        ```xml hl_lines="1"
        <ModOp Property="ModuleOwner"
               Merge=".[FarmType='PlantFarm']">
          <ModuleOwner>
            <ModuleBuildRadius>20</ModuleBuildRadius>
          </ModuleOwner>
        </ModOp>
        ```
    === ":material-animation-outline: 117 & 1800"
        ```xml hl_lines="1"
        <ModOp Type="merge"
               Path="//ModuleOwner/[FarmType='PlantFarm']">
          <ModuleOwner>
            <ModuleBuildRadius>20</ModuleBuildRadius>
          </ModuleOwner>
        </ModOp>
        ```

## Extra Notes

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

