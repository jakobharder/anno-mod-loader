# Modinfo.json Specification

This page describes `modinfo.json` fields the modloader uses.
Check the full [modinfo.json specification](https://github.com/anno-mods/Modinfo) for general overview.

Field | Description
--- | ---
Version | Used to select the newest version of the same ModID.
ModID | Used for dependencies and duplicate detection.<br/>The folder name is used if a mod has no ModID / modinfo.json
DeprecateIds | Ignores mods listed in DeprecateIds. Bug: it currently prints an error if a dependency requires a deprecated mod.
IncompatibleIds | Prints errors if incompatible mods are found and active.
ModDependencies | Prints errors if listed mods are not active or found.
LoadAfterIds | Load mod after listed mods.<br/>See [modloader10#loading-order](./modloader10.md#loading-order) for how it works.

Example:
```json
{
  "Version": "1.4.4",
  "ModID": "jakob_new_world_cities",
  "DeprecateIds": [
    "jakob_nw_construction",
    "jakob_bugfix_celluloid",
    "jakob_power_nw"
  ],
  "ModDependencies": [
    "shared-pools-and-definitions"
  ],
  "LoadAfterIds": [
    "shared-pools-and-definitions",
    "Spice_Harborlife",
    "FamTaubeLexy_NewWorldHotels"
  ]
}
```