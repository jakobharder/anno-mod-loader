# Mod Profiles

You can change the activation of a mod in the following ways:

- Recommended: Prefix entry with `#` in `<user>/Anno 117/mods/active-profile.txt` {{a117}}
- Prefix mod with `-` {{all}}
- Add disable entry in `<mods>/activation.json` {{a1800}}

## Profile File

{{a117r}}

Prefix an entry with `#` to disable it.

```shell title="&lt;user&gt;/Anno 117/mods/active-profile.txt" hl_lines="9"
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

The loader adds all detected mods to the active profile.
Prefix `EnableNewMods` with `#` to disable newly added mods by default.

```shell title="active-profile.txt" hl_lines="1"
# EnableNewMods
```

## Prefix With Minus

{{all}}

Add a `-` in front of a mod folder or zip file.

```plaintext title="mods/" hl_lines="2"
├─ mod-a
├─ -mod-b
└─ mod-c
```

!!! warning "Limitations"
    It is not recommended to change while the game is running.

    The mod.io folder does not support this method.

## Activations File

{{a1800}}

Place a `activation.json` file in your install or user documents `mods/` folder and add entries like the following:

```json title="&lt;mods&gt;/activation.json"
{
  "disabledIds": [
    "ModA",
    "ModB"
  ]
}
```

`ModID` from `modinfo.json` is used, and if not specified the folder name of the mod as a fallback.

## Account Data

{{a1800}}

TBD