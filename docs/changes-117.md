# Changes with Anno 117

There are a few changes in file structure and modinfo.json that are mandatory.

### Mod Folders

The locations of the mod folders for manual installation are unchanged.

- `<user>/Anno 117/mods/`
- `<install>/Anno 117/mods/`

### Mod Profiles

!!! warning "Profiles are not available in the demo"

You can change the activation of a mod in two ways:

- Prefix entry with `#` in `<user>/Anno 117/active-profile.txt`
- Prefix folder with `-` *(not recommended while the game is running)*

The loader adds all detected mods to the active profile:

```ini title="active-profile.txt"
## Disable a mod:
## - Add a `#` before it.
##
## Disable mods not listed:
## - Add `#` before `EnableNewMods` to automatically disable new mods.
## - That's useful if you want to backup your activation list as a mod profile.
EnableNewMods

# infinite-money-ubi
infinite-materials-ubi
```

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

### Changed Paths

The location of the main `assets.xml` has changed.

=== ":material-pillar: 117"
    `data\base\config\export\assets.xml`
=== ":material-factory: 1800"
    `data\config\export\main\asset\assets.xml`

Most base game files are also under `data\base` now.

!!! tip "Templates in Visual Studio Code"
    Press ++f1++ in [Anno Modding Tools](https://marketplace.visualstudio.com/items?itemName=JakobHarder.anno-modding-tools) and run `Anno: Create Mod from Template` to generate a template.

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

### Modinfo

The `modinfo.json` now supports JSON with comments (you can also name it `modinfo.jsonc`).

A game version entry is now mandatory:

```json title="modinfo.jsonc"
{
  "Anno": 8,
  // ...
}
```

!!! warning "Mods without modinfo and game version won't be loaded"

There are a few more informational entries in `modinfo.json`.

```json title="modinfo.jsonc"
{
  "Anno": 8,
  // ...
  // new entries:
  "Difficulty": "harder",
  "GameSetup": {
    "RequiresNewGame": false,
    "SafeToRemove": false,
    "Multiplayer": true,
    "Campaign": true
  }
}
```

|Name|Value|Effect|
|---|---|---|
|`Anno` (mandatory)[^1]|`8`|Only mods with the correction version will be loaded
|`Difficulty` (mandatory)[^2]|`cheat`|e.g. no construction costs
|.|`easier`|makes the game easier, e.g. reduced needs consumption
|.|`unchanged`|is balanced similar as the vanilla game, e.g. new productions or quality of life features like free farmfield placement
|.|`harder`|makes the game harder
|`RequiresNewGame`|`true` or `false`|only works with a new savegame, for example like river slots.<br/>Default is `false`.
|`SafeToRemove`|`true` or `false`|can be removed from a savegame without leaving trails. For example construction menu reordering.<br/>Default is `false`.
|`Multiplayer`|`true` or `false`|can be used in multiplayer games.<br/>Default is `true`.
|`Campaign`|`true` or `false`|can be used in campaign games.<br/>Default is `true`.

[^1]: The mod won't be loaded without the game version entry.
[^2]: The mod loader prints an error if the entry is missing or wrong.

## InfoTips

!!! warning "Not available in demo"

There are a few changes to improve readability:

1. Types and Operators have proper names instead of numbers.
2. ElementType is now an attribute of InfoElement and VisibilityElement.
3. Nested operators like `CompareOperator` are now more compact.
4. No `ChildCount`s anymore

=== ":material-pillar: 117"
    ```xml
    <InfoElement Type="Container">
      <VisibilityElement Type="Group">
        <VisibilityElement Type="Condition">
          <CompareOperator>Greater</CompareOperator>
          <!-- .. -->
        </VisibilityElement>
      </VisibilityElement>
      <!-- .. -->
    </InfoElement>
    ```

=== ":material-factory: 1800"
    ```xml
    <InfoElement>
      <ElementType>23</ElementType>
      <ChildCount>1</ChildCount>
      <VisibilityElement>
        <ElementType>
          <ElementType>2</ElementType>
        <ElementType>
        <VisibilityElement>
          <ElementType>
            <ElementType>1</ElementType>
          <ElementType>
          <ChildCount>1</ChildCount>
          <CompareOperator>
            <CompareOperator>5</CompareOperator>
          </CompareOperator>
          <!-- .. -->
        </VisibilityElement>
      </VisibilityElement>
      <!-- .. -->
    </InfoElement>
    ```

Note: These improvements are not part of [FileDBReader](https://github.com/anno-mods/FileDBReader), except the type and operator names. Keep that in mind when comparing **FileDBReader** and **xmltest** output.


!!! info "Reminder: Texts use OasisIDs now (`TextId`) instead of GUIDs (`TextGUID`)"
