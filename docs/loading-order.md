# Mod Loading order

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