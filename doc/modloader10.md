# ModLoader 10 Changes

- Copy Existing Nodes with `Content`
- Loading Order

## Copy Existing Nodes with `Content`

```xml
<ModOp Type="replace" GUID="1500010225"
       Path="/Properties/Building/InfluencedVariationDirection"
       Content="//Values[Standard/GUID='1500010200']/Building/InfluencedVariationDirection" />
```

## Error on missing dependency

Mods mentioned in `ModDependencies` but not available and active in the `mods/` will print an error in the logs now.

## Loading order

You can specificy `LoadAfterIds` in `modinfo.json` now to load your mod after another.

No warning will printed if the mentioned mod is not available.
Use `ModDependencies` to signal required mods.

```json
{
  "ModID": "ModC",
  "LoadAfterIds": [
    "ModA",
    "ModB"
  ]
}
```

```json
{
  "ModID": "PostB",
  "LoadAfterIds": [
    "*",
    "PostA"
  ]
}
```

The order is as follows:
1. Mods with `LoadAfterIds` but without `*` following the order. Alphabetically order is ignored.
2. Mods without `LoadAfterIds` loaded alphabetically.
3. Mods with `*` loaded in order of `LoadAfterIds`.

A mod without a `*` cannot load after a mod with a `*`.

Note: Step 2 ensures previous behavior as long as there's no `LoadAfterIds`.

## Mods in sub-folders

```yaml
- mods/
  - Collection/
    - ModA/
      - data/
      - modinfo.json
    - ModB/
      - data/
      - modinfo.json
```

Collection folders do not act as mods.
They don't have activation or `ModID`s.

A mod manager may bulk enable/disable based on those folders though...

`modinfo.json` is required for mods in sub-folders.

## Mods in zip files

```yaml
- mods/
  - Mod.zip/
    - data/
    - modinfo.json
```

```yaml
- mods/
  - Collection.zip/
    - ModA/
      - data/
      - modinfo.json
    - ModB/
      - data/
      - modinfo.json
```

`modinfo.json` is required for mods in zips.

## Shared Mods within Mods

Perfect for shared data. You can version your shared data now to make sure the latest copy across all mods is used.

```yaml
- mods/
  - ModA/
    - data/
    - modinfo.json
    - SharedMod/
      - data/
      - modinfo.json
  - ModB/
    - data/
    - modinfo.json
    - SharedMod/
      - data/
      - modinfo.json
```

`modinfo.json` is required for such mods.

### Make part of your mod load at the end

Sub-mods also follow loading order.
You can use that fact to add a few ModOps to be executed after other mods, rather than loading your complete mod at the end.

## Activation via `activation.json` in mods folder

With enabling mods in zips an alternative way to deactivate mods is needed.

You can do that now with the following json file in your `mods/` folder:

```json
{
  "disabledIds": [
    "ModA",
    "ModB"
  ]
}
```

`ModID` from `modinfo.json` is used, and if not specified the folder name of the mod as a fallback.
