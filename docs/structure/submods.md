# Sub-Mods

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
