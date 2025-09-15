# Modloader Reference

## Changes in Anno 117

<div class="annotate" markdown>
-   [Changed Paths](./file-structure.md#basic-file-structure) - Paths to `assets.xml` and others have changed. (1)
-   [Modinfo](./modinfo.md#mandatory-fields) - Only mods with game version `8` are loaded. (2)
-   No changes to [GUID Ranges](#safe-ranges) (3) or [Mod Folders](./install.md#mod-folders). (4)
</div>

1.  Most base game files are also under `data/base` now.

    E.g. `data/base/config/export/assets.xml`

2.  Only mods with game version `8` in `modinfo.json` are loaded.
    ```json
    {
    "Anno": 8,
    "Version": "1.0.0",
    "ModID": "first-mod-jakob"
    // ..
    }
    ```

3.  Reserve your GUID range.

    Open a PR on [GuidRanges](https://github.com/anno-mods/GuidRanges) or ask on [Discord](https://discord.gg/CUq2zQdV).

4.  The mod folders for manual installation are:

    - `<user>/Anno 117/mods/`
    - `<install>/Anno 117/mods/`

Read the [full list of changes](./changes-117.md).

??? tip "Templates in Visual Studio Code"
    Press ++f1++ in [Anno Modding Tools](https://marketplace.visualstudio.com/items?itemName=JakobHarder.anno-modding-tools) and run `Anno: Create Mod from Template` to generate a template.

## Create Mods

<div class="grid cards" markdown>
-   [__File Structure__](./file-structure.md.md)
-   [__Modinfo__](./modinfo.md)
-   [__Loading Order__](./loading-order.md)
</div>

<div class="grid cards" markdown>
-   [__Get Started__](./modops/index.md)
-   [__Basic Types__](./modops/basics.md)
-   [__Groups and Conditions__](./modops/control.md)
-   [__Dynamic Content__](./modops/content.md)
-   [__Item Lists__](./modops/lists.md)
</div>

<div class="grid cards" markdown>
-   [__InfoTips__](./infotips.md)
-   [__CFG and FC__](./cfg-fc-patches.md)
</div>

<div class="grid cards" markdown>
-   [__Options__](./options.md)
-   [__Lua Scripts__](./lua-scripts.md)
</div>

## Use Mods

<div class="grid cards" markdown>
-   [__Install Mods__](./install.md)
-   [__Mod Profiles__](./profiles.md)
</div>

## External

<div class="grid cards" markdown>

-   [__XPath Cheatsheet__](https://devhints.io/xpath)

</div>