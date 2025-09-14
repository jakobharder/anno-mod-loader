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
