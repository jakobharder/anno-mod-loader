# ModLoader 10 Changes

- Copy Existing Nodes with `Content`

## Copy Existing Nodes with `Content`

```xml
<ModOp Type="replace" GUID="1500010225"
       Path="/Properties/Building/InfluencedVariationDirection"
       Content="//Values[Standard/GUID='1500010200']/Building/InfluencedVariationDirection" />
```

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
