# Modinfo Specification

A `modinfo.json` file is required in the top-level folder of your mod.

!!! warning "Anno 117 mods and Anno 1800 mod.io mods won't load without a modinfo file."

??? tip "Auto-completion and Template in Visual Studio Code"
    Use [Anno Modding Tools](https://marketplace.visualstudio.com/items?itemName=JakobHarder.anno-modding-tools) to get template, auto-completion and tooltips for `modinfo.json` files.

    Ensure [anno.workspaceSchemas](vscode://settings/anno.workspaceSchemas) is enabled for auto-completion and tooltips.

    Press ++f1++ and run `Anno: Create Mod from Template` to generate a template.

## Mandatory Fields

{{all}}

Mandatory fields are used by the modloader to correctly identify mods. Missing or wrong entries result in errors in the `mod-loader.log`.

!!! warning "Anno 117 won't load mods without the correct game version."

```json title="modinfo.json"
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

```json title="modinfo.json"
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
    ```json
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
    |`LoadAfter`|List mods to load before this mod.
    |`Deprecate`|List mods that are replaced by this mod.<br/>A warning will be printed in the mod-loader.log. LoadAfter and Require will use the new ID.
    |`Incompatible`|List incompatible mods.<br/>Active incompatible mods result in log warning.

=== ":material-factory: 1800"
    ```json
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
    |`LoadAfterIds`|List mods to load before this mod.
    |`DeprecateIds`|List mods that are replaced by this mod.
    |`IncompatibleIds`|List incompatible mods.<br/>Active incompatible mods result in log warning.

## Development Info

{{all}}

Infos used by Visual Studio Code plugin [Anno Modding Tools](https://marketplace.visualstudio.com/items?itemName=JakobHarder.anno-modding-tools).

These don't depend on the game version, but tool version.

=== ":material-animation-outline: Current"
    ```json
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
    ```json
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
    The Visual Studio Code [modding tools](https://marketplace.visualstudio.com/items?itemName=JakobHarder.anno-modding-tools) can deploy your mods with automatic DDS generation, dependency downloads, etc.

    Check the [Build and Deploy](https://github.com/anno-mods/vscode-anno-modding-tools/blob/main/doc/utilities.md#build-and-deploy-mods) documentation.

## Localized Info

TBD

## Additional Info

TBD