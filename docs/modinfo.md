# Modinfo Specification

A `modinfo.json` file is required in the top-level folder of your mod.

!!! warning "Anno 117 mods and Anno 1800 mod.io mods won't load without a modinfo file."

??? tip "Auto-completion and Template in Visual Studio Code"
    Use [Anno Modding Tools](https://anno-mods.github.io/vscode-anno/) to get template, auto-completion and tooltips for `modinfo.json` files.

    Ensure [anno.workspaceSchemas](vscode://settings/anno.workspaceSchemas) is enabled for auto-completion and tooltips.

    Press ++f1++ and run `Anno: Create Mod from Template` to generate a template.

## Mandatory Fields

{{all}}

Mandatory fields are used by the modloader to correctly identify mods. Missing or wrong entries result in errors in the `mod-loader.log`.

!!! warning "Anno 117 won't load mods without the correct game version."

```json title="modinfo.jsonc"
{
    "Anno": 8,
    "Version": "1.0.0",
    "ModID": "first-mod-jakob"
    // ..
}
```

Field | Description
--- | ---
`Anno` | `8` for Anno 117.<br/>`7` for Anno 1800 (optional but recommended for better tool support).
`Version` | Version with three numbers: `MAJOR.Minor.patch`.<br/><br/>`MAJOR`: increase when a new game is recommended. Use `0` if you don't guarantee not breaking save games.<br/>`Minor`: increase for feature additions.<br/>`patch`: bugfixes only.
`ModID` | Unique identifier of the mod.<br/>Use dashes and append the creator, e.g. `modname-creatorname`.<br/><br/>1800 only: the folder name is used if a mod has no `ModID` or `modinfo.json`.

## Game Setup

{{a117}}

```json title="modinfo.jsonc"
{
    // ..
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
|`Difficulty`[^difficulty]|`cheat`|e.g. no construction costs<br/><br/>__Mandatory for Anno 117__
|.|`easier`|makes the game easier, e.g. reduced needs consumption
|.|`unchanged`|is balanced similar as the vanilla game, e.g. new productions or quality of life features like free farmfield placement
|.|`harder`|makes the game harder
|`RequiresNewGame`|`true` or `false`|only works with a new savegame, for example like river slots.<br/>Default is `false`.
|`SafeToRemove`|`true` or `false`|can be removed from a savegame without leaving trails. For example construction menu reordering.<br/>Default is `false`.
|`Multiplayer`|`true` or `false`|can be used in multiplayer games.<br/>Default is `true`.
|`Campaign`|`true` or `false`|can be used in campaign games.<br/>Default is `true`.

[^difficulty]: The mod loader prints an error if the entry is missing or wrong.

## Dependencies

{{all}}

=== ":material-pillar: 117"
    ```json title="modinfo.jsonc"
    {
        // ..
        "Dependencies": {
            "Require": [],
            "Optional": [],
            "LoadAfter": [],
            "Deprecate": [],
            "Incompatible": []
        }
    }
    ```

    |Name|Description
    |---|---
    |`Require`|List mods and DLCs that are required to use this mod.<br/>Missing dependencies result in log warning.
    |`Optional`|List mods and DLCs that enable further content in this mod.
    |`LoadAfter`|List mods to load before this mod.<br/>See also [loading order](./loading-order.md).
    |`Deprecate`|List mods that are replaced by this mod.<br/>A warning will be printed in the mod-loader.log. LoadAfter and Require will use the new ID.
    |`Incompatible`|List incompatible mods.<br/>Active incompatible mods result in log warning.

=== ":material-factory: 1800"
    ```json title="modinfo.jsonc"
    {
        // ..
        "ModDependencies": [],
        "OptionalDependencies": [],
        "LoadAfterIds": [],
        "DeprecateIds": [],
        "IncompatibleIds": []
    }
    ```

    |Name|Description
    |---|---
    |`ModDependencies`|List mods that are required to use this mod.<br/>Missing dependencies result in log warning.
    |`OptionalDependencies`|List mods that enable further content in this mod.
    |`LoadAfterIds`|List mods to load before this mod.<br/>See also [loading order](./loading-order.md).
    |`DeprecateIds`|List mods that are replaced by this mod.
    |`IncompatibleIds`|List incompatible mods.<br/>Active incompatible mods result in log warning.

## Development Info

{{all}}

Infos used by Visual Studio Code plugin [Anno Modding Tools](https://anno-mods.github.io/vscode-anno/).

These don't depend on the game version, but tool version.

=== ":material-animation-outline: Current"
    ```json title="modinfo.jsonc"
    {
        // ..
        "Development": {
            "Dependencies": [],
            "DeployPath": "${annoMods}/${modName}",
            "Bundle": []
        }
    }
    ```

    |Name|Description
    |---|---
    |`Dependencies`|List of mods that should be loaded for patch error analysis and GUID references.
    |`DeployPath`|Target path for mod deployment.
    |`Bundle`|List of mods that should be included as a sub mod.
=== ":material-close-octagon-outline: Deprecated"
    ```json title="modinfo.jsonc"
    {
        // ..
        "OptionalDependencies": [],
        "out": "${annoMods}/${modName}",
        "bundle": []
    }
    ```

    |Name|Description
    |---|---
    |`OptionalDependencies`|List of mods that should be loaded for patch error analysis and GUID references.
    |`out`|Target path for mod deployment.
    |`bundle`|List of mods that should be included as a sub mod.

??? tip "Learn more about deploying mods."
    The Visual Studio Code [modding tools](https://anno-mods.github.io/vscode-anno/) can deploy your mods with automatic DDS generation, dependency downloads, etc.

    Check the [Deploy Mods](https://anno-mods.github.io/vscode-anno/tools/deploy/) documentation.

## Localized Info

{{all}}

Use language-keyed objects for `Category`, `ModName`, and `Description`:

```json title="modinfo.jsonc"
{
    // ..
    "Category": {
        "English": "Buildings",
        "German": "Gebäude"
    },
    "ModName": {
        "English": "My Mod",
        "German": "Meine Mod"
    },
    "Description": {
        "English": "Adds a building."
    }
}
```

The [VS Code schema](https://github.com/anno-mods/vscode-anno/blob/main/languages/modinfo-schema.json)
requires `English` in these objects. Common language keys are `English`, `French`,
`German`, `Italian`, `Japanese`, `Korean`, `Polish`, `Russian`, and `Spanish`.

Language | Anno 117 | Anno 1800
--- | --- | ---
Simplified Chinese | `ChineseSimplified` | `Chinese`
Traditional Chinese | `ChineseTraditional` | `Taiwanese`
Brazilian Portuguese | `Brazilian` | —

VS Code requires `Category` and `ModName`; `Description` is optional.

## Additional Info

Optional metadata fields:

Field | Format
--- | ---
`CreatorName` | Creator name as a string.
`CreatorContact` | Contact link as a string, e.g. a repository URL.
`KnownIssues` | Array of localized text objects, e.g. `[ { "English": "Known issue." } ]`. {{a1800}}
`DLCDependencies` | Array of objects with `DLC` and `Dependant` fields. Use auto-completion for DLC names; `Dependant` accepts `required`, `partly`, or `atLeastOneRequired`. {{a1800}}
`ModioResourceId` | Numeric mod.io resource ID. {{a1800}}
`Custom` | Object for custom metadata; its contents are not validated. {{a117}}

```json title="modinfo.jsonc"
{
    // ..
    "CreatorName": "Your Name",
    "CreatorContact": "https://github.com/your-name/your-mod"
}
```
